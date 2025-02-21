#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment( lib, "ws2_32.lib" )

#include "DxLib.h"


// �T�[�o��IP�A�h���X
const char* SERVER_ADDRESS = "127.0.0.1";
// �T�[�o�̃|�[�g�ԍ�
const unsigned short SERVER_PORT = 8888;

struct CIRCLE
{
	int centerX;
	int centerY;
	int size;
	int color;
};




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//
	WSADATA	wsaData;
	// WinSock2.2 ����������
	if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 )
	{
		return 0;
	}

	// �\�P�b�g�̍쐬
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( sock == INVALID_SOCKET )
	{
		return 0;
	}
	
	// �m���u���b�L���O���[�h
	u_long arg = 0x01;
	ioctlsocket( sock, FIONBIO, &arg );

	// �T�[�o�A�h���X�̎w��
	SOCKADDR_IN serverAddr;
	memset( &serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( SERVER_PORT );
	inet_pton( AF_INET, SERVER_ADDRESS, &serverAddr.sin_addr.s_addr);

	
	//
	
	//SetWindowText( "�N���C�A���g" );
	SetGraphMode( 800, 600, 32 );
	ChangeWindowMode( TRUE );
	
	if( DxLib_Init() == -1 )
		return -1;							
	
	SetBackgroundColor( 0, 0, 0 );
	SetDrawScreen( DX_SCREEN_BACK );
	SetAlwaysRunFlag( 1 );

	while(1)
	{
		ClearDrawScreen();

		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
		{

			// �����l...��ʂ͈̔͊O
			CIRCLE circle2 = { -100, -100, 0, GetColor(255, 255, 255) };


			// ��M
			int ret = recvfrom(sock, (char*)&circle2, sizeof(circle2), 0, nullptr, nullptr);
			if (ret > 0)
			{
				// �o�C�g�I�[�_�[�̕ϊ�
				circle2.centerX = ntohl(circle2.centerX);
				circle2.centerY = ntohl(circle2.centerY);
				circle2.size = ntohl(circle2.size);
				circle2.color = ntohl(circle2.color);
			}
			else if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				// ��M���s�̃G���[����
			}

			// �G
			DrawCircle(circle2.centerX, circle2.centerY, circle2.size, circle2.color, 1);
		}
		else
		{

			CIRCLE circle = { 0, 0, 5, GetColor(0,255,255) };

			// �}�E�X�ʒu�Ɂ��`��
			GetMousePoint(&circle.centerX, &circle.centerY);
			DrawCircle(circle.centerX, circle.centerY, circle.size, circle.color, 1);


			// �T�[�o�Ɂ��̏�񑗐M...�֐������悤
			CIRCLE sendData = { htonl(circle.centerX), htonl(circle.centerY), htonl(circle.size), htonl(circle.color) };
			int ret = sendto(sock, (char*)&sendData, sizeof(sendData), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));


			if (ret == SOCKET_ERROR)
			{
				// �G���[����
				// break;
			}
		}
		
		ScreenFlip();
		WaitTimer( 16 );
		if( ProcessMessage() == -1 || CheckHitKey(KEY_INPUT_ESCAPE) == 1 )
		{
			break;
		}
	}

	DxLib_End();
	return 0;
}