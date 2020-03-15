/* rls.c -	リモートディレクトリ表示サービスのクライアント
 * 					使い方：rls <ホスト名> <ディレクトリ>
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#define oops(msg)	{ perror(msg); exit(1); }
#define PORTNUM		15000

int main(int ac, char *av[])
{
	struct	sockaddr_in		servadd;								/* 呼び出す番号									*/
	struct	hostent				*hp;										/* 番号の取得										*/
	int			sock_id, sock_fd;											/* ソケットとfd									*/
	char		buffer[BUFSIZ];												/* 受信メッセージ								*/
	int			n_read;																/* メッセージサイズ							*/

	if (ac != 3) exit(1);

	/* 
	 * ステップ1：ソケットを作成する
	 */

	sock_id = socket(AF_INET, SOCK_STREAM, 0);		/* 回線を取得する								*/
	if ( sock_id == -1)
		oops("socket");															/* 失敗													*/

	/* 
	 * ステップ2：サーバに接続する
	 */

	bzero(&servadd, sizeof(servadd));							/* アドレスを0クリアする				*/
	hp = gethostbyname(av[1]);										/* ホストのIPアドレスを照合する	*/
	if (hp == NULL)
		oops(av[1]);																/* 失敗													*/
	bcopy(hp->h_addr, (struct sockaddr *)&servadd.sin_addr, hp->h_length);
	servadd.sin_port = htons(PORTNUM);						/* ポート番号を設定							*/
	servadd.sin_family = AF_INET;									/* ソケットタイプを設定する			*/

	if (connect(sock_id, (struct sockaddr *)&servadd, sizeof(servadd)) != 0)
		oops("connect");

	/*
	 * ステップ3：ディレクトリ名を送り、結果を読み出す
	 */

	if (write(sock_id, av[2], strlen(av[2])) == -1)
		oops("write");
	if (write(sock_id, "\n", 1) == -1)
		oops("write");

	while ((n_read = read(sock_id, buffer, BUFSIZ)) > 0)
		if (write(1, buffer, n_read) == -1)
			oops("write");
	close(sock_id);
}

