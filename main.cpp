#include <Windows.h>
#include <iostream>
#include <list>
#include <chrono>
using namespace std;

int telaLargura = 120;
int telaAltura = 30;

struct partesSnake {
    int x;
    int y;
};

struct coordComida {
    int x;
    int y;
};

int main() {
    //Screen Buffer
    wchar_t* tela = new wchar_t[telaLargura * telaAltura];
    for (int i = 0; i < telaAltura * telaLargura; i++) {
        tela[i] = L' ';
    }
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;


    while (1) {
        //Inicializando as coordenadas da cobra
        list<partesSnake> cobra = { { 60,15 }, { 61,15 }, { 62,15 }, { 63,15 }, { 64,15 }, { 65,15 }, { 66,15 }, { 67,15 }, { 68,15 }, { 69,15 } };
        //Inicializa a direção da cobra 0==cima | 1==direita | 2==baixo | 3==esquerda
        int direcaoCobra = 3;
        //Inicializando a coordenada comida
        coordComida comida = { 30, 15 };
        //Controle de estado da cobra
        bool estaMorto = false;
        //Pontuação total do jogador
        int pontuacao = 0;
        //Controle de teclas
        bool setaDireita = false, setaDireitaAnterior = false, setaEsquerda = false, setaEsquerdaAnterior = false;

        while (!estaMorto) {
            auto t1 = chrono::system_clock::now();
            while ((chrono::system_clock::now() - t1) < ((direcaoCobra % 2 == 1) ? 120ms : 200ms))
            {
                //Recebe as entradas do teclado
                setaDireita = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
                setaEsquerda = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;

                if (setaDireita && !setaDireitaAnterior) {
                    direcaoCobra = (direcaoCobra + 1) % 4;
                }
                if (setaEsquerda && !setaEsquerdaAnterior) {
                    direcaoCobra--;
                    if (direcaoCobra == -1) {
                        direcaoCobra = 3;
                    }
                }
                setaDireitaAnterior = setaDireita;
                setaEsquerdaAnterior = setaEsquerda;
            }

            //Movimentando a cobra
            switch (direcaoCobra)
            {
            case 0:
                cobra.push_front({ cobra.front().x, cobra.front().y - 1 });
                break;
            case 1:
                cobra.push_front({ cobra.front().x + 1, cobra.front().y });
                break;
            case 2:
                cobra.push_front({ cobra.front().x, cobra.front().y + 1 });
                break;
            case 3:
                cobra.push_front({ cobra.front().x - 1, cobra.front().y });
                break;
            }
            cobra.pop_back();

            //Verifica se a cobra colidiu com alguma das bordas
            if (cobra.front().x < 0 || cobra.front().x >= telaLargura - 1 || cobra.front().y < 3 || cobra.front().y >= telaAltura) {
                estaMorto = true;
            }

            //Verifica se a cobra comeu a comida
            if (cobra.front().x == comida.x && cobra.front().y == comida.y) {
                pontuacao++;
                while (tela[comida.y * telaLargura + comida.x] != L' ') {
                    comida.x = rand() % telaLargura;
                    comida.y = rand() % (telaAltura - 3) + 3;
                }

                for (int i = 0; i < 5; i++) {
                    cobra.push_back({ cobra.back().x, cobra.back().y });
                }
            }

            //Verifica se a cobra colidiu com ela mesma
            for (list<partesSnake>::iterator i = cobra.begin(); i != cobra.end(); i++) {
                if (i != cobra.begin() && i->x == cobra.begin()->x && i->y == cobra.begin()->y) {
                    estaMorto = true;
                }
            }


            //Limpa a tela
            for (int i = 0; i < telaAltura * telaLargura; i++) {
                tela[i] = L' ';
            }

            //Desenha a pontuação e a borda
            for (int i = 0; i < telaLargura; i++) {
                tela[i] = L'=';
                tela[2 * telaLargura + i] = L'=';
            }
            wsprintfW(&tela[telaLargura], L"    Bem vindo ao jogo!         Pontos: %d", pontuacao);

            //Desenha a cobra
            for (auto c : cobra) {
                tela[c.y * telaLargura + c.x] = estaMorto ? L'+' : L'O';
            }
            tela[cobra.front().y * telaLargura + cobra.front().x] = estaMorto ? L'X' : L'@';

            if (estaMorto) {
                wsprintf(&tela[(telaLargura * telaAltura) / 2 - 16 + telaLargura / 2], L"Presssione espaco para continuar");
            }

            //Desenha a comida
            tela[comida.y * telaLargura + comida.x] = L'#';

            WriteConsoleOutputCharacterW(hConsole, tela, telaLargura * telaAltura, { 0,0 }, &dwBytesWritten);
        }
        while (!GetAsyncKeyState(VK_SPACE)) {

        }
    }

    return 0;
}
