# Chatserver.cの説明

## 概要
複数クライアント間でのメッセージ交換サーバを作成しました。通信にはsocketシステムコール，I/Oの多重化にはselectシステムコールを使用しました。

## 主な仕様
1. 1つのクライアントが返事を送ると，自分以外の全クライアントにその返事を表示します．
2. 1つのプロセスで2つ以上のクライアントの接続を受け付け，他のクライアントに送信を行います．
3. 指定数以下なら任意個のクライアント接続を受け付けます(新規接続と切断にも対応)．

## 作成した関数
実装するに当たって作成した関数の簡単な動作の説明を示します．
- main()<br>
プログラムの最初に呼び出される関数です．
- Message()<br>
void 型の関数であり，clients[i]をint型の変数connfdとして引数にとり，クライアントが入力した文字を他のクライアントに送信します．
- Delete()<br>
void型の関数であり，関数Messageから呼び出されます．
Message()内のconnfdをint型の変数connfdとして引数にとり，接続が切れたクライアントに対応するソケット記述子を取り除きます．

## 使用した主な変数
以下に使用した主な変数とその変数の型名および説明を箇条書きで示します．
- maxclients<br>
int 型の変数であり，同時に接続できるクライアントの最大数を格納します。この値を変更することによって最大クライアント数を変更することが可能です．
- clients<br>
int 型の配列であり，maxclients で指定した値だけ長さをとります．1つ1つの要素には接続済みのソケット記述子を格納します．
- nclients<br>
int 型の変数であり，現在接続しているクライアントの数を示します．
- servaddr<br>
struct sockaddr in 型の構造体変数です．今回はIPv4を想定して通信を行うためこの型にしました．
- listenfd<br>
int 型の変数であり，socket()から返される待ち受け用のソケットです．
- connfd<br>
int 型の変数であり，accept()から返される通信用のソケットです．
- maxfd<br>
int 型の変数であり，接続済みのソケット記述子のうちの最大数を示す．
- readfds<br>
select() で使用するためのfd set型の引数であり，読みが可能になった記述子群を表す．
- size<br>
int 型の変数であり，一度の入力で読み込める文字数を指定します．
- buf<br>
char 型の配列であり，長さは変数 size の値だけとります．クライアントが入力した文字を格納します．

## 動作実行例
chatserver.cをコンパイルし、実行させてみます。<br>次の画像はプログラムを実行しクライアントからの接続を待受している状況です。<br>
<img width="500" alt="aaaaa" src="https://github.com/ykodera03/ykodera/blob/master/img/server-sample1.png">
<br>
クライアントの動作確認にはtelnetコマンドしました。<br>
<img width="500" alt="aaaaa" src="https://github.com/ykodera03/ykodera/blob/master/img/client-sample1.png">
<br>
<br>
クライアントからの接続をサーバ側で確認しました。接続のあったクライアントにはfd=4という識別子が割り振られていることが分かります。<br>
<img width="500" alt="aaaaa" src="https://github.com/ykodera03/ykodera/blob/master/img/server-sample2.png">
<br>
<br>
2人目のクライアントからも接続をしてみました。
今度はサーバ側でfd=5という識別子が割り振られました。
<br>
<img width="500" alt="aaaaa" src="https://github.com/ykodera03/ykodera/blob/master/img/server-sample3.png">
<br>
<br>
fd=4のクライアントから何か発言をしてみます。
<br>
<img width="500" alt="aaaaa" src="https://github.com/ykodera03/ykodera/blob/master/img/client-sample2.png">
<br>
<br>
fd=5のクライアントにもメッセージが届いていることが確認できました。<br>
(ユーザ側で確認できる識別子を表示させていないので画像上からは分からないですね汗)
<br>
<img width="500" alt="aaaaa" src="https://github.com/ykodera03/ykodera/blob/master/img/client-sample3.png">
<br>
<br>
fd=4から接続を終了してみます。<br>
<img width="500" alt="aaaaa" src="https://github.com/ykodera03/ykodera/blob/master/img/client-sample4.png">
<br>
<br>
サーバ側では、fd=4のクライアントが接続を終了したことを確認できました。<br>
<img width="500" alt="aaaaa" src="https://github.com/ykodera03/ykodera/blob/master/img/server-sample4.png">
