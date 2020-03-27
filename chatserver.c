#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

/* メッセージ交換サーバー*/
/* 必須要件 */
/* 
   ・1つのクライアントからの接続を待つ
   ・クライアントがメッセージを1行送ると, 何らかの返事を1行返す
*/


const int maxclients=5; /* 最大クライアント数 */
const int size = 1024;
int clients[maxclients]; 
int nclients = 0;

char *newmsg = "Welcome to Chatroom!!.\n\nLet's send a message!.\n";
char *overmsg = "string is too long.\n";
char *fullclients ="It's full. Sorry.\n";

void Message(int connfd);
void Delete(int connfd);

int main (void){

  int listenfd,connfd;
  
  struct sockaddr_in servaddr;
  /* ソケットを作成 */
  if((listenfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    perror ("socket"); exit(1);
  }
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET; //重要
  servaddr.sin_port = htons(10000); //ポート番号
  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  
  /* ポートをバインド */
  if(bind (listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror ("bind"); exit(1);
  }
  
  /* キューの長さを決定 */
  if(listen (listenfd, 5) < 0) {
    perror ("listen"); exit(1);
  }

  fprintf(stderr, "Ready.\n");
  
  for(;;) {
    int  i,maxfd;
    fd_set readfds;
 

     /* selectのための記述子集合を作る */
     FD_ZERO(&readfds); /* 空集合を作る */
     FD_SET(listenfd, &readfds); /* 接続待ちソケットを見張る */
     maxfd = listenfd;

     /* 繋がっているクライアントからの発言がないか見張る */
     for (i = 0; i < nclients; i++){
       FD_SET(clients[i], &readfds);
       if(clients[i] > maxfd){
         maxfd = clients[i];
       }
     }

     /* いずれかの記述子の準備ができるまで待つ */
     if (select(maxfd+1, &readfds, NULL, NULL, NULL) < 0){
       perror("select");
       exit(1);
     }

     
     /* 新しい接続があればacceptする */
     if(FD_ISSET(listenfd, &readfds)){  
       /* コネクションを待つ */
       if ((connfd = accept (listenfd,  (struct sockaddr *)NULL, NULL)) < 0) {
         perror("accept"); exit(1);
       }
       fprintf(stderr, "accept\n");
       
       if(!(nclients >= maxclients)){ //最大接続数に達していないなら
         clients[nclients] = connfd; /* 記述子集合に加える */
         nclients++;
         fprintf(stderr, "Accepted a connection on descriptor %d\n",connfd);
         write(connfd,newmsg,strlen(newmsg));
       } else { //もう満杯
         write(connfd, fullclients, strlen(fullclients)); 
         close(connfd);
         fprintf(stderr, "It's full.\n");
       }
     }
     

    /* 発言があれば全クライアントに送る */
     for(i = 0; i < nclients; i++){
       //配列に格納したconnfd(ソケット記述子)があれば
       if (FD_ISSET(clients[i], &readfds)) { 
	 /* msg start */
         Message(clients[i]);
       }
     }
     
  }
}

void Message(int connfd){
  int nbytes, own;
  char buf [size];
  
  /* connfdから大きさ分読んでbufに格納,読んだバイト数をnbytesに返す */
  if((nbytes = read (connfd, buf, sizeof(buf))) > 0) {
    if(nbytes < sizeof(buf)){
      own = connfd; /* 自分のソケット記述子を記録 */
    }else{
      write(connfd,overmsg, strlen(overmsg));
      fprintf(stderr,"Over specified number of characters on descriptor %d\n.",connfd);
      return;
    }

  } else if (nbytes == -1){
    perror("read");
  } else if (nbytes == 0){ //EOF 相手が通信路を切ったら
    close(connfd);
    fprintf(stderr, "Closed on connection on descriptor %d\n",connfd);
    Delete(connfd);
  }
  
  fwrite (buf, 1, nbytes, stderr); //サーバ側で確認しているだけ
  for (int i = 0; i < nclients; i++){

    if(own != clients[i]){ // 自分以外のクライアントの場合
      write(clients[i], buf, nbytes); //bufからnbytes取ってきて各クライアントに送信
    }
  }
}

void Delete(int connfd){
  for (int i = 0; i < nclients; i++) {
      if(clients[i] == connfd){
	clients[i] = clients[nclients-1]; //現在最後尾のconnfdを切れた配列に移動
	nclients--; // 現在の接続数を1つ減らす
      }  
    }
}
