/* PROJETO LP1 - ADS1S2014 IFSP-Campinas- 2014/06/27
    NOME: CEZAR SILVA  - PROT. 140006-1
    NOME: FAUSTO SOUSA - PROT. 140017-7
*/

// Incluindo o SDL.h, SDL_mixer.h e o resto.
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>

/* Enumeracao do booleano:
    Util, pois nao existe bool em C.
    Sera bastante usado.
/* --------------  */

enum boolean
{
    true = 1, false = 0
};
typedef enum boolean bool;

/* --------------  */

/* Structs uteis para o jogo.
    A maioria para armazenamento de posicoes.
/* --------------  */

// Struct de posicao dos inimigos.
typedef struct
{
    int x;
    int y;
} ponto;

// Struct do vetor de pontos que define a trajetoria dos inimigos.
typedef struct
{
    ponto posicao[900];
} trajetoria;

/* --------------  */


/* Constantes:
    Diversas constantes do programa, como resolucao, passo e tempo.
    E interessante colocar todas aqui.
/* --------------  */

// Resolucao, X/Y.
const int tela_largura = 800;
const int tela_altura = 600;

// Passo padrao da nave.
const int passo = 5;

// Tempo de delay padrao em MS.
const int delay = 10;

// Duracao das fases em MS.
const int duracao = 60000;

// Area de tiro.
const int area = 40;

/* --------------  */


/* Controles de Fase e Tempo.
    Essas variaveis precisam ser globais.
/* --------------  */

// Relogio do jogo.
long relogio = 0;

// Numero de fase (apresentacao, fase 1, 2, 3 e final).
int fase = 0;

// Indica se deve mudar de fase.
bool muda = false;

// Espera de fase.
bool espera = true;

// Booleana de exibicao da nave.
bool bnave = false;

/* --------------  */


/* Variaveis globais do SDL.
    Variaveis para janela, fundo, nave, etc.
    Alem disso, ha variavel de retangulo.
    Futuramente, tera audio.
/* --------------  */

// Janela que vamos usar para renderizar os graficos.
SDL_Window* janela = NULL;

// Superficie da janela.
SDL_Surface* superficie_inicial = NULL;

// Superficie onde vamos carregar a imagem de background.
SDL_Surface* superficie_fundo = NULL;

// Superficie da textura do aviao.
SDL_Surface* superficie_nave = NULL;

// Retangulo que define tamanho da nave e localizacao.
SDL_Rect retangulo;

// Musica de fundo.
Mix_Music* musica = NULL;

// Som do disparo.
Mix_Chunk* sdisparo = NULL;

// Som da explosao.
Mix_Chunk* sexplosao = NULL;

// Struct com imagens, retangulos e booleana dos inimigos.
typedef struct
{
    SDL_Surface* super_i1;
    SDL_Rect ret1;
    bool i1;

    SDL_Surface* super_i2;
    SDL_Rect ret2;
    bool i2;

    SDL_Surface* super_i3;
    SDL_Rect ret3;
    bool i3;

    SDL_Surface* super_i4;
    SDL_Rect ret4;
    bool i4;

    SDL_Surface* super_i5;
    SDL_Rect ret5;
    bool i5;
} fase_i;

// Variavel dos inimigos.
fase_i inimigos;

// Struct com as variaveis do disparos (cinco total).
typedef struct
{
    SDL_Surface* surf_disparo1;
    SDL_Rect ret_disparo1;
    bool exibir_disparo1;

    SDL_Surface* surf_disparo2;
    SDL_Rect ret_disparo2;
    bool exibir_disparo2;

    SDL_Surface* surf_disparo3;
    SDL_Rect ret_disparo3;
    bool exibir_disparo3;

    SDL_Surface* surf_disparo4;
    SDL_Rect ret_disparo4;
    bool exibir_disparo4;

    SDL_Surface* surf_disparo5;
    SDL_Rect ret_disparo5;
    bool exibir_disparo5;

    int quantos;
} disparos;

// Variavel do disparo.
disparos tiros;

// Struct com as variaveis de exibicao de pontos.
typedef struct
{
    bool exibir;
    int quantos;

    SDL_Surface* surf_ponto1;
    SDL_Rect ret_ponto1;

    SDL_Surface* surf_ponto2;
    SDL_Rect ret_ponto2;

    SDL_Surface* surf_ponto3;
    SDL_Rect ret_ponto3;

} pontuacao;

// Variavel do placar.
pontuacao placar;

/* --------------  */


/* Threads e boleanos de controle
    Essas variáveis precisam ser globais.
    Necessário incluir no fechamento para fechar as threads.
*/

// Thread do console de debug.
SDL_Thread* tdebug;
bool bdebug = true;

// Thread de desenho dos graficos.
SDL_Thread* tdesenha;
bool bdesenha = true;

// Thread de controle do disparo.
SDL_Thread* tcontrole;
bool bdisparo = true;

// Thread de disparo.
SDL_Thread* tdisparo;

// Thread da fase demo.
SDL_Thread* tfase;
bool bfase = false;


/* --------------  */


// Carrega o SDL e retorna resultado.
bool carrega_sdl()
{
    // Tenta inicializar o SDL e ja faz a verificacao.
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL nao pode ser inicializado! SDL_Error: %s\n", SDL_GetError());
		return false;
    }
	else
	{
	    // Tenta inicializar o SDL Mixer e ja faz a verificacao.
	    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        {
            printf("SDL Mixer nao pode ser inicializado! SDL_mixer Error: %s\n", Mix_GetError());
            return false;
        }
        else
        {
            return true;
        }
	}
}

// Carrega janela do Windows para SDL e retorna resultado.
bool carrega_janela()
{
    // Tenta criar a janela.
    janela = SDL_CreateWindow("Interstella 5555",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    tela_largura,
                                    tela_altura,
                                    SDL_WINDOW_SHOWN);
    // Verificacao.
    if (janela == NULL)
    {
        printf("Janela nao pode ser criada! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        return true;
    }
}

// Carrega imagens iniciais e retorna resultado.
bool carrega_midia()
{
        // Pegando a superfície da janela.
        superficie_inicial = SDL_GetWindowSurface(janela);

        // Carrega a imagem de fundo.
        superficie_fundo = SDL_LoadBMP("graphic/inicio/logo.bmp");

        // Carrega imagem do aviao.
        superficie_nave = SDL_LoadBMP("graphic/inicio/nave.bmp");

        // Remove a cor branca da aviao.
        Uint32 branco = SDL_MapRGB(superficie_nave->format, 255, 255, 255);
        SDL_SetColorKey(superficie_nave, 1, branco);

        // Carrega imagem e remove cor branca do disparo.
        tiros.surf_disparo1 = SDL_LoadBMP("graphic/disparo/disparo1.bmp");
        Uint32 brancot = SDL_MapRGB(tiros.surf_disparo1->format, 255, 255, 255);
        SDL_SetColorKey(tiros.surf_disparo1, 1, brancot);

        tiros.surf_disparo2 = SDL_LoadBMP("graphic/disparo/disparo2.bmp");
        //Uint32 brancot = SDL_MapRGB(tiros.surf_disparo2->format, 255, 255, 255);
        SDL_SetColorKey(tiros.surf_disparo2, 1, brancot);

        tiros.surf_disparo3 = SDL_LoadBMP("graphic/disparo/disparo3.bmp");
        //Uint32 brancot = SDL_MapRGB(tiros.surf_disparo3->format, 255, 255, 255);
        SDL_SetColorKey(tiros.surf_disparo3, 1, brancot);

        tiros.surf_disparo4 = SDL_LoadBMP("graphic/disparo/disparo4.bmp");
        //Uint32 brancot = SDL_MapRGB(tiros.surf_disparo4->format, 255, 255, 255);
        SDL_SetColorKey(tiros.surf_disparo4, 1, brancot);

        tiros.surf_disparo5 = SDL_LoadBMP("graphic/disparo/disparo5.bmp");
        //Uint32 brancot = SDL_MapRGB(tiros.surf_disparo5->format, 255, 255, 255);
        SDL_SetColorKey(tiros.surf_disparo5, 1, brancot);

        tiros.exibir_disparo1 = false;
        tiros.exibir_disparo2 = false;
        tiros.exibir_disparo3 = false;
        tiros.exibir_disparo4 = false;
        tiros.exibir_disparo5 = false;

        musica = Mix_LoadMUS("audio/apresentacao.mp3");

        sdisparo = Mix_LoadWAV("audio/disparo.wav");

        sexplosao = Mix_LoadWAV("audio/explosao.wav");

        // Checagem e retorno de resultado.
        if(superficie_inicial != NULL)
        {
            if(superficie_fundo != NULL)
            {
                if(superficie_nave != NULL)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
}

/* Rotina de saida do jogo
    Fim do programa.
    Rotinas de desalocação de memoria do SDL e seus sistemas.
*/
void fechamento()
{
    bdesenha = false;
    bdisparo = false;

    // Desaloca as superficies.
    SDL_FreeSurface(superficie_inicial);
    superficie_inicial = NULL;
    SDL_FreeSurface(superficie_fundo);
    superficie_fundo = NULL;
    SDL_FreeSurface(superficie_nave);
    superficie_nave = NULL;

    // Inimigos.
    SDL_FreeSurface(inimigos.super_i1);
    inimigos.super_i1 = NULL;
    SDL_FreeSurface(inimigos.super_i2);
    inimigos.super_i2 = NULL;
    SDL_FreeSurface(inimigos.super_i3);
    inimigos.super_i3 = NULL;
    SDL_FreeSurface(inimigos.super_i4);
    inimigos.super_i4 = NULL;
    SDL_FreeSurface(inimigos.super_i5);
    inimigos.super_i5 = NULL;

    // Disparos.
    SDL_FreeSurface(tiros.surf_disparo1);
    tiros.surf_disparo1 = NULL;
    SDL_FreeSurface(tiros.surf_disparo2);
    tiros.surf_disparo2 = NULL;
    SDL_FreeSurface(tiros.surf_disparo3);
    tiros.surf_disparo3 = NULL;
    SDL_FreeSurface(tiros.surf_disparo4);
    tiros.surf_disparo4 = NULL;
    SDL_FreeSurface(tiros.surf_disparo5);
    tiros.surf_disparo5 = NULL;

    // Audio
    Mix_HaltMusic();
    Mix_FreeMusic(musica);
    Mix_FreeChunk(sdisparo);
    Mix_FreeChunk(sexplosao);

	// Destroi a janela.
	SDL_DestroyWindow(janela);
	janela = NULL;

	// Sai do subsistema do SDL.
	SDL_Quit();

	// Sai do subsistema do SDL Mixer.
	Mix_Quit();
}

// Faz a animacao da tela com a nave e inimigos
void desenha()
{
    while (bdesenha)
    {
        // Coloca o fundo.
        SDL_BlitSurface(superficie_fundo, NULL, superficie_inicial, NULL);

        // Coloca o aviao.
        if (bnave)
        {
            SDL_BlitSurface(superficie_nave, NULL, superficie_inicial, &retangulo);
        }

        // Coloca os disparos.
        if (tiros.exibir_disparo1)
        {
            SDL_BlitSurface(tiros.surf_disparo1, NULL, superficie_inicial, &tiros.ret_disparo1);
        }
        if (tiros.exibir_disparo2)
        {
            SDL_BlitSurface(tiros.surf_disparo2, NULL, superficie_inicial, &tiros.ret_disparo2);
        }
        if (tiros.exibir_disparo3)
        {
            SDL_BlitSurface(tiros.surf_disparo3, NULL, superficie_inicial, &tiros.ret_disparo3);
        }
        if (tiros.exibir_disparo4)
        {
            SDL_BlitSurface(tiros.surf_disparo4, NULL, superficie_inicial, &tiros.ret_disparo4);
        }
        if (tiros.exibir_disparo5)
        {
            SDL_BlitSurface(tiros.surf_disparo5, NULL, superficie_inicial, &tiros.ret_disparo5);
        }

        // Verifica se tem inimigo e coloca (cada um dos 5).
        if(inimigos.i1)
        {
            SDL_BlitSurface(inimigos.super_i1, NULL, superficie_inicial, &inimigos.ret1);
        }
        if(inimigos.i2)
        {
            SDL_BlitSurface(inimigos.super_i2, NULL, superficie_inicial, &inimigos.ret2);
        }
        if(inimigos.i3)
        {
            SDL_BlitSurface(inimigos.super_i3, NULL, superficie_inicial, &inimigos.ret3);
        }
        if(inimigos.i4)
        {
            SDL_BlitSurface(inimigos.super_i4, NULL, superficie_inicial, &inimigos.ret4);
        }
        if(inimigos.i5)
        {
            SDL_BlitSurface(inimigos.super_i5, NULL, superficie_inicial, &inimigos.ret5);
        }

        // Exibicao do placar
        if(placar.exibir)
        {
            // Unidade
            switch(placar.quantos % 10)
            {
                case 0:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/0.bmp");
                        break;
                    }
                case 1:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/1.bmp");
                        break;
                    }
                case 2:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/2.bmp");
                        break;
                    }
                case 3:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/3.bmp");
                        break;
                    }
                case 4:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/4.bmp");
                        break;
                    }
                case 5:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/5.bmp");
                        break;
                    }
                case 6:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/6.bmp");
                        break;
                    }
                case 7:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/7.bmp");
                        break;
                    }
                case 8:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/8.bmp");
                        break;
                    }
                case 9:
                    {
                        SDL_FreeSurface(placar.surf_ponto1);
                        placar.surf_ponto1 = NULL;
                        placar.surf_ponto1 = SDL_LoadBMP("graphic/numeros/9.bmp");
                        break;
                    }
            }

            // Dezena
            switch(((placar.quantos % 100) - (placar.quantos % 10)) / 10)
            {
                case 0:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/0.bmp");
                        break;
                    }
                case 1:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/1.bmp");
                        break;
                    }
                case 2:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/2.bmp");
                        break;
                    }
                case 3:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/3.bmp");
                        break;
                    }
                case 4:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/4.bmp");
                        break;
                    }
                case 5:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/5.bmp");
                        break;
                    }
                case 6:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/6.bmp");
                        break;
                    }
                case 7:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/7.bmp");
                        break;
                    }
                case 8:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/8.bmp");
                        break;
                    }
                case 9:
                    {
                        SDL_FreeSurface(placar.surf_ponto2);
                        placar.surf_ponto2 = NULL;
                        placar.surf_ponto2 = SDL_LoadBMP("graphic/numeros/9.bmp");
                        break;
                    }
            }

            switch ((placar.quantos - (placar.quantos % 100)) / 100) // Centena
            {
                case 0:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/0.bmp");
                        break;
                    }
                case 1:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/1.bmp");
                        break;
                    }
                case 2:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/2.bmp");
                        break;
                    }
                case 3:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/3.bmp");
                        break;
                    }
                case 4:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/4.bmp");
                        break;
                    }
                case 5:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/5.bmp");
                        break;
                    }
                case 6:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/6.bmp");
                        break;
                    }
                case 7:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/7.bmp");
                        break;
                    }
                case 8:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/8.bmp");
                        break;
                    }
                case 9:
                    {
                        SDL_FreeSurface(placar.surf_ponto3);
                        placar.surf_ponto3 = NULL;
                        placar.surf_ponto3 = SDL_LoadBMP("graphic/numeros/9.bmp");
                        break;
                    }
            }

            SDL_BlitSurface(placar.surf_ponto1, NULL, superficie_inicial, &placar.ret_ponto1);
            SDL_BlitSurface(placar.surf_ponto2, NULL, superficie_inicial, &placar.ret_ponto2);
            SDL_BlitSurface(placar.surf_ponto3, NULL, superficie_inicial, &placar.ret_ponto3);

        }

        // Atualiza a janela.
        SDL_UpdateWindowSurface(janela);

        // Delay para ilusao de animacao.
        SDL_Delay(delay);
    }

}

// Funcao de disparo.
void atirar()
{
    int intervalo = 150;

    int alinhamento = 20;

    if (!tiros.exibir_disparo1)
    {
        tiros.exibir_disparo1 = true;
        tiros.ret_disparo1.x = retangulo.x + alinhamento;
        tiros.ret_disparo1.y = retangulo.y;
    }

    SDL_Delay(intervalo);
    if (!tiros.exibir_disparo2)
    {
        tiros.exibir_disparo2 = true;
        tiros.ret_disparo2.x = retangulo.x + alinhamento;
        tiros.ret_disparo2.y = retangulo.y;
    }
    SDL_Delay(intervalo);
    if (!tiros.exibir_disparo3)
    {
        tiros.exibir_disparo3 = true;
        tiros.ret_disparo3.x = retangulo.x + alinhamento;
        tiros.ret_disparo3.y = retangulo.y;
    }
    SDL_Delay(intervalo);
    if (!tiros.exibir_disparo4)
    {
        tiros.exibir_disparo4 = true;
        tiros.ret_disparo4.x = retangulo.x + alinhamento;
        tiros.ret_disparo4.y = retangulo.y;
    }
    SDL_Delay(intervalo);
    if (!tiros.exibir_disparo5)
    {
        tiros.exibir_disparo5 = true;
        tiros.ret_disparo5.x = retangulo.x + alinhamento;
        tiros.ret_disparo5.y = retangulo.y;
    }
}

// Funcao de controle de tiro.
void controle_tiro()
{
    while(bdisparo)
    {
        // Passo dos disparos
        if (tiros.exibir_disparo1)
        {
            tiros.ret_disparo1.y = tiros.ret_disparo1.y - passo;
            if (tiros.ret_disparo1.y <= 0)
            {
                tiros.exibir_disparo1 = false;
            }
        }
        if (tiros.exibir_disparo2)
        {
            tiros.ret_disparo2.y = tiros.ret_disparo2.y - passo;
            if (tiros.ret_disparo2.y <= 0)
            {
                tiros.exibir_disparo2 = false;
            }
        }
        if (tiros.exibir_disparo3)
        {
            tiros.ret_disparo3.y = tiros.ret_disparo3.y - passo;
            if (tiros.ret_disparo3.y <= 0)
            {
                tiros.exibir_disparo3 = false;
            }
        }
        if (tiros.exibir_disparo4)
        {
            tiros.ret_disparo4.y = tiros.ret_disparo4.y - passo;
            if (tiros.ret_disparo4.y <= 0)
            {
                tiros.exibir_disparo4 = false;
            }
        }
        if (tiros.exibir_disparo5)
        {
            tiros.ret_disparo5.y = tiros.ret_disparo5.y - passo;
            if (tiros.ret_disparo5.y <= 0)
            {
                tiros.exibir_disparo5 = false;
            }
        }

        // Controle de colisao
        // Inimigo 1
        if (
            (tiros.exibir_disparo1) && (
            ((inimigos.ret1.x < (tiros.ret_disparo1.x + area)) && (inimigos.ret1.x > (tiros.ret_disparo1.x - area)) &&
            (inimigos.ret1.y < (tiros.ret_disparo1.y + area)) && (inimigos.ret1.y > (tiros.ret_disparo1.y - area)))
            ||
            ((inimigos.ret1.x < (tiros.ret_disparo2.x + area)) && (inimigos.ret1.x > (tiros.ret_disparo2.x - area)) &&
            (inimigos.ret1.y < (tiros.ret_disparo2.y + area)) && (inimigos.ret1.y > (tiros.ret_disparo2.y - area)))
            ||
            ((inimigos.ret1.x < (tiros.ret_disparo3.x + area)) && (inimigos.ret1.x > (tiros.ret_disparo3.x - area)) &&
            (inimigos.ret1.y < (tiros.ret_disparo3.y + area)) && (inimigos.ret1.y > (tiros.ret_disparo3.y - area)))
            ||
            ((inimigos.ret1.x < (tiros.ret_disparo4.x + area)) && (inimigos.ret1.x > (tiros.ret_disparo4.x - area)) &&
            (inimigos.ret1.y < (tiros.ret_disparo4.y + area)) && (inimigos.ret1.y > (tiros.ret_disparo4.y - area)))
            ||
            ((inimigos.ret1.x < (tiros.ret_disparo5.x + area)) && (inimigos.ret1.x > (tiros.ret_disparo5.x - area)) &&
            (inimigos.ret1.y < (tiros.ret_disparo5.y + area)) && (inimigos.ret1.y > (tiros.ret_disparo5.y - area))) )
            )
        {
            if (inimigos.i1)
            {
                inimigos.i1 = false;
                Mix_PlayChannel(-1, sexplosao, 0);
                placar.quantos++;
            }

        }
        // Inimigo 2
        if (
            (tiros.exibir_disparo2) && (
            ((inimigos.ret2.x < (tiros.ret_disparo1.x + area)) && (inimigos.ret2.x > (tiros.ret_disparo1.x - area)) &&
            (inimigos.ret2.y < (tiros.ret_disparo1.y + area)) && (inimigos.ret2.y > (tiros.ret_disparo1.y - area)))
            ||
            ((inimigos.ret2.x < (tiros.ret_disparo2.x + area)) && (inimigos.ret2.x > (tiros.ret_disparo2.x - area)) &&
            (inimigos.ret2.y < (tiros.ret_disparo2.y + area)) && (inimigos.ret2.y > (tiros.ret_disparo2.y - area)))
            ||
            ((inimigos.ret2.x < (tiros.ret_disparo3.x + area)) && (inimigos.ret2.x > (tiros.ret_disparo3.x - area)) &&
            (inimigos.ret2.y < (tiros.ret_disparo3.y + area)) && (inimigos.ret2.y > (tiros.ret_disparo3.y - area)))
            ||
            ((inimigos.ret2.x < (tiros.ret_disparo4.x + area)) && (inimigos.ret2.x > (tiros.ret_disparo4.x - area)) &&
            (inimigos.ret2.y < (tiros.ret_disparo4.y + area)) && (inimigos.ret2.y > (tiros.ret_disparo4.y - area)))
            ||
            ((inimigos.ret2.x < (tiros.ret_disparo5.x + area)) && (inimigos.ret2.x > (tiros.ret_disparo5.x - area)) &&
            (inimigos.ret2.y < (tiros.ret_disparo5.y + area)) && (inimigos.ret2.y > (tiros.ret_disparo5.y - area))) )
            )
        {
            if (inimigos.i2)
            {
                inimigos.i2 = false;
                Mix_PlayChannel(-1, sexplosao, 0);
                placar.quantos++;
            }
        }
        // Inimigo 3
        if (
            (tiros.exibir_disparo3) && (
            ((inimigos.ret3.x < (tiros.ret_disparo1.x + area)) && (inimigos.ret3.x > (tiros.ret_disparo1.x - area)) &&
            (inimigos.ret3.y < (tiros.ret_disparo1.y + area)) && (inimigos.ret3.y > (tiros.ret_disparo1.y - area)))
            ||
            ((inimigos.ret3.x < (tiros.ret_disparo2.x + area)) && (inimigos.ret3.x > (tiros.ret_disparo2.x - area)) &&
            (inimigos.ret3.y < (tiros.ret_disparo2.y + area)) && (inimigos.ret3.y > (tiros.ret_disparo2.y - area)))
            ||
            ((inimigos.ret3.x < (tiros.ret_disparo3.x + area)) && (inimigos.ret3.x > (tiros.ret_disparo3.x - area)) &&
            (inimigos.ret3.y < (tiros.ret_disparo3.y + area)) && (inimigos.ret3.y > (tiros.ret_disparo3.y - area)))
            ||
            ((inimigos.ret3.x < (tiros.ret_disparo4.x + area)) && (inimigos.ret3.x > (tiros.ret_disparo4.x - area)) &&
            (inimigos.ret3.y < (tiros.ret_disparo4.y + area)) && (inimigos.ret3.y > (tiros.ret_disparo4.y - area)))
            ||
            ((inimigos.ret3.x < (tiros.ret_disparo5.x + area)) && (inimigos.ret3.x > (tiros.ret_disparo5.x - area)) &&
            (inimigos.ret3.y < (tiros.ret_disparo5.y + area)) && (inimigos.ret3.y > (tiros.ret_disparo5.y - area))) )
            )
        {
            if (inimigos.i3)
            {
                inimigos.i3 = false;
                Mix_PlayChannel(-1, sexplosao, 0);
                placar.quantos++;
            }
        }
        // Inimigo 4
        if (
            (tiros.exibir_disparo4) && (
            ((inimigos.ret4.x < (tiros.ret_disparo1.x + area)) && (inimigos.ret4.x > (tiros.ret_disparo1.x - area)) &&
            (inimigos.ret4.y < (tiros.ret_disparo1.y + area)) && (inimigos.ret4.y > (tiros.ret_disparo1.y - area)))
            ||
            ((inimigos.ret4.x < (tiros.ret_disparo2.x + area)) && (inimigos.ret4.x > (tiros.ret_disparo2.x - area)) &&
            (inimigos.ret4.y < (tiros.ret_disparo2.y + area)) && (inimigos.ret4.y > (tiros.ret_disparo2.y - area)))
            ||
            ((inimigos.ret4.x < (tiros.ret_disparo3.x + area)) && (inimigos.ret4.x > (tiros.ret_disparo3.x - area)) &&
            (inimigos.ret4.y < (tiros.ret_disparo3.y + area)) && (inimigos.ret4.y > (tiros.ret_disparo3.y - area)))
            ||
            ((inimigos.ret4.x < (tiros.ret_disparo4.x + area)) && (inimigos.ret4.x > (tiros.ret_disparo4.x - area)) &&
            (inimigos.ret4.y < (tiros.ret_disparo4.y + area)) && (inimigos.ret4.y > (tiros.ret_disparo4.y - area)))
            ||
            ((inimigos.ret4.x < (tiros.ret_disparo5.x + area)) && (inimigos.ret4.x > (tiros.ret_disparo5.x - area)) &&
            (inimigos.ret4.y < (tiros.ret_disparo5.y + area)) && (inimigos.ret4.y > (tiros.ret_disparo5.y - area))) )
            )
        {
            if (inimigos.i4)
            {
                inimigos.i4 = false;
                Mix_PlayChannel(-1, sexplosao, 0);
                placar.quantos++;
            }
        }
        // Inimigo 5
        if ((tiros.exibir_disparo5) && (
            ((inimigos.ret5.x < (tiros.ret_disparo1.x + area)) && (inimigos.ret5.x > (tiros.ret_disparo1.x - area)) &&
            (inimigos.ret5.y < (tiros.ret_disparo1.y + area)) && (inimigos.ret5.y > (tiros.ret_disparo1.y - area)))
            ||
            ((inimigos.ret5.x < (tiros.ret_disparo2.x + area)) && (inimigos.ret5.x > (tiros.ret_disparo2.x - area)) &&
            (inimigos.ret5.y < (tiros.ret_disparo2.y + area)) && (inimigos.ret5.y > (tiros.ret_disparo2.y - area)))
            ||
            ((inimigos.ret5.x < (tiros.ret_disparo3.x + area)) && (inimigos.ret5.x > (tiros.ret_disparo3.x - area)) &&
            (inimigos.ret5.y < (tiros.ret_disparo3.y + area)) && (inimigos.ret5.y > (tiros.ret_disparo3.y - area)))
            ||
            ((inimigos.ret5.x < (tiros.ret_disparo4.x + area)) && (inimigos.ret5.x > (tiros.ret_disparo4.x - area)) &&
            (inimigos.ret5.y < (tiros.ret_disparo4.y + area)) && (inimigos.ret5.y > (tiros.ret_disparo4.y - area)))
            ||
            ((inimigos.ret5.x < (tiros.ret_disparo5.x + area)) && (inimigos.ret5.x > (tiros.ret_disparo5.x - area)) &&
            (inimigos.ret5.y < (tiros.ret_disparo5.y + area)) && (inimigos.ret5.y > (tiros.ret_disparo5.y - area))) )
            )
        {
            if (inimigos.i5)
            {
                inimigos.i5 = false;
                Mix_PlayChannel(-1, sexplosao, 0);
                placar.quantos++;
            }
        }

        SDL_Delay(delay);
    }
}

// Gerador de trajeto aleatorio para inimigos.
trajetoria aleatorio(int numero)
{
    trajetoria caminho;
    int i;
    int baixo = 1;
    int lados = 1;
    int direcao = 1;
    srand(time(NULL));

    switch(numero)
    {
        case 1:
            {
                caminho.posicao[0].x = rand() % tela_largura;
                break;
            }
        case 2:
            {
                caminho.posicao[0].x = (rand() * rand()) % tela_largura;
                break;
            }
        case 3:
            {
                caminho.posicao[0].x = (rand() * rand() * rand()) % tela_largura;
                break;
            }
        case 4:
            {
                caminho.posicao[0].x = (rand() * rand() * rand() * rand()) % tela_largura;
                break;
            }
        case 5:
            {
                caminho.posicao[0].x = (rand() * rand() * rand() * rand() * rand()) % tela_largura;
                break;
            }
    }

    caminho.posicao[0].y = 0;

    for (i = 1; i < 900; i++)
    {

        switch(lados)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                {
                    lados++;
                    caminho.posicao[i].x = caminho.posicao[i - 1].x + direcao;
                    break;
                }
            case 11:
                {
                    lados = 1;
                    if (rand()%2)
                    {
                        direcao = -1;
                    }
                    else
                    {
                        direcao = 1;
                    }
                    caminho.posicao[i].x = caminho.posicao[i - 1].x + direcao;
                    break;
                }
        }

        switch(baixo)
        {
            case 1:
            case 2:
                {
                    caminho.posicao[i].y = caminho.posicao[i - 1].y + 1;
                    baixo++;
                    break;
                }
            case 3:
                {
                    caminho.posicao[i].y = caminho.posicao[i - 1].y;
                    baixo = 1;
                    break;
                }
        }

    }

    return caminho;
}

// Funcao de tempo, para limitar o tempo do loop no mesmo tempo das threads.
void tempo()
{
    // Delay e acrescimo ao relogio do jogo.
    SDL_Delay(delay);
    relogio = relogio + delay;
}

// Limita posicao X/Y da nave.
void limita_posicao()
{
    // Limitacao de cenario
    if (retangulo.x >= tela_largura - 55)
    {
        retangulo.x = retangulo.x - passo;
    }
    if (retangulo.y >= tela_altura - 55)
    {
        retangulo.y = retangulo.y - passo;
    }
    if (retangulo.x <= 0)
    {
        retangulo.x = retangulo.x + passo;
    }
    if (retangulo.y <= 0)
    {
        retangulo.y = retangulo.y + passo;
    }
}

// Funcao de fase.
void ffase()
{
    // Apresentacao da fase
    bnave = false;
    placar.exibir = false;
    switch(fase)
    {
        case 1:
            {
                superficie_fundo = SDL_LoadBMP("graphic/fase1/inicio1.bmp");
                break;
            }
        case 2:
            {
                superficie_fundo = SDL_LoadBMP("graphic/fase2/inicio2.bmp");
                break;
            }
        case 3:
            {
                superficie_fundo = SDL_LoadBMP("graphic/fase3/inicio3.bmp");
                break;
            }
    }
    SDL_Delay(4000);
    placar.exibir = true;

    // Ativando o loop e a nave.
    bfase = true;
    bnave = true;

    // Zerando o relogio do jogo.
    relogio = 0;

    // Fundo da fase.
    switch(fase)
    {
        case 1:
            {
                superficie_fundo = SDL_LoadBMP("graphic/fase1/fase1.bmp");
                break;
            }
        case 2:
            {
                superficie_fundo = SDL_LoadBMP("graphic/fase2/fase2.bmp");
                break;
            }
        case 3:
            {
                superficie_fundo = SDL_LoadBMP("graphic/fase3/fase3.bmp");
                break;
            }
    }

    // Musica da fase
    switch(fase)
    {
        case 1:
            {
                musica = Mix_LoadMUS("audio/fase1.mp3");
                break;
            }
        case 2:
            {
                musica = Mix_LoadMUS("audio/fase2.mp3");
                break;
            }
        case 3:
            {
                musica = Mix_LoadMUS("audio/fase3.mp3");
                break;
            }
    }
    Mix_PlayMusic(musica, -1);

    // Inimigos da fase.
    // 1
    SDL_FreeSurface(inimigos.super_i1);
    inimigos.super_i1 = NULL;
    switch(fase)
    {
        case 1:
            {
                inimigos.super_i1 = SDL_LoadBMP("graphic/fase1/1.bmp");
                break;
            }
        case 2:
            {
                inimigos.super_i1 = SDL_LoadBMP("graphic/fase2/1.bmp");
                break;
            }
        case 3:
            {
                inimigos.super_i1 = SDL_LoadBMP("graphic/fase3/1.bmp");
                break;
            }
    }
    Uint32 branco1 = SDL_MapRGB(inimigos.super_i1->format, 255, 255, 255);
    SDL_SetColorKey(inimigos.super_i1, 1, branco1);
    inimigos.ret1.h = 64;
    inimigos.ret1.w = 64;
    inimigos.ret1.x = 400;
    inimigos.ret1.y = 0;
    inimigos.i1 = false;

    // 2
    SDL_FreeSurface(inimigos.super_i2);
    inimigos.super_i2 = NULL;
    switch(fase)
    {
        case 1:
            {
                inimigos.super_i2 = SDL_LoadBMP("graphic/fase1/2.bmp");
                break;
            }
        case 2:
            {
                inimigos.super_i2 = SDL_LoadBMP("graphic/fase2/2.bmp");
                break;
            }
        case 3:
            {
                inimigos.super_i2 = SDL_LoadBMP("graphic/fase3/2.bmp");
                break;
            }
    }
    Uint32 branco2 = SDL_MapRGB(inimigos.super_i2->format, 255, 255, 255);
    SDL_SetColorKey(inimigos.super_i2, 1, branco2);
    inimigos.ret2.h = 64;
    inimigos.ret2.w = 64;
    inimigos.ret2.x = 400;
    inimigos.ret2.y = 0;
    inimigos.i2 = false;

    // 3
    SDL_FreeSurface(inimigos.super_i3);
    inimigos.super_i3 = NULL;
    switch(fase)
    {
        case 1:
            {
                inimigos.super_i3 = SDL_LoadBMP("graphic/fase1/3.bmp");
                break;
            }
        case 2:
            {
                inimigos.super_i3 = SDL_LoadBMP("graphic/fase2/3.bmp");
                break;
            }
        case 3:
            {
                inimigos.super_i3 = SDL_LoadBMP("graphic/fase3/3.bmp");
                break;
            }
    }
    Uint32 branco3 = SDL_MapRGB(inimigos.super_i3->format, 255, 255, 255);
    SDL_SetColorKey(inimigos.super_i3, 1, branco3);
    inimigos.ret3.h = 64;
    inimigos.ret3.w = 64;
    inimigos.ret3.x = 400;
    inimigos.ret3.y = 0;
    inimigos.i3 = false;

    // 4
    SDL_FreeSurface(inimigos.super_i4);
    inimigos.super_i4 = NULL;
    switch(fase)
    {
        case 1:
            {
                inimigos.super_i4 = SDL_LoadBMP("graphic/fase1/4.bmp");
                break;
            }
        case 2:
            {
                inimigos.super_i4 = SDL_LoadBMP("graphic/fase2/4.bmp");
                break;
            }
        case 3:
            {
                inimigos.super_i4 = SDL_LoadBMP("graphic/fase3/4.bmp");
                break;
            }
    }
    Uint32 branco4 = SDL_MapRGB(inimigos.super_i4->format, 255, 255, 255);
    SDL_SetColorKey(inimigos.super_i4, 1, branco4);
    inimigos.ret4.h = 64;
    inimigos.ret4.w = 64;
    inimigos.ret4.x = 400;
    inimigos.ret4.y = 0;
    inimigos.i4 = false;

    // 5
    SDL_FreeSurface(inimigos.super_i5);
    inimigos.super_i5 = NULL;
    switch(fase)
    {
        case 1:
            {
                inimigos.super_i5 = SDL_LoadBMP("graphic/fase1/5.bmp");
                break;
            }
        case 2:
            {
                inimigos.super_i5 = SDL_LoadBMP("graphic/fase2/5.bmp");
                break;
            }
        case 3:
            {
                inimigos.super_i5 = SDL_LoadBMP("graphic/fase3/5.bmp");
                break;
            }
    }
    Uint32 branco5 = SDL_MapRGB(inimigos.super_i5->format, 255, 255, 255);
    SDL_SetColorKey(inimigos.super_i5, 1, branco5);
    inimigos.ret5.h = 64;
    inimigos.ret5.w = 64;
    inimigos.ret5.x = 400;
    inimigos.ret5.y = 0;
    inimigos.i5 = false;

    // Trajetorias e auxiliar.
    trajetoria caminho1, caminho2, caminho3, caminho4, caminho5;
    int i;

    // Loop de fase.
    while (bfase)
    {

        caminho1 = aleatorio(1);
        inimigos.i1 = true;
        caminho2 = aleatorio(2);
        inimigos.i2 = true;
        caminho3 = aleatorio(3);
        inimigos.i3 = true;
        caminho4 = aleatorio(4);
        inimigos.i4 = true;
        caminho5 = aleatorio(5);
        inimigos.i5 = true;

        for(i = 0; i < 900; i++)
        {
            inimigos.ret1.x = caminho1.posicao[i].x;
            inimigos.ret1.y = caminho1.posicao[i].y;

            inimigos.ret2.x = caminho2.posicao[i].x;
            inimigos.ret2.y = caminho2.posicao[i].y;

            inimigos.ret3.x = caminho3.posicao[i].x;
            inimigos.ret3.y = caminho3.posicao[i].y;

            inimigos.ret4.x = caminho4.posicao[i].x;
            inimigos.ret4.y = caminho4.posicao[i].y;

            inimigos.ret5.x = caminho5.posicao[i].x;
            inimigos.ret5.y = caminho5.posicao[i].y;

            SDL_Delay(delay);
        }

        if (relogio > duracao)
        {
            bfase = false;
        }
    }
    inimigos.i1 = false;
    inimigos.i2 = false;
    inimigos.i3 = false;
    inimigos.i4 = false;
    inimigos.i5 = false;

    Mix_HaltMusic();
    muda = true;
    fase++;

}

// Funcao de apresentacao da tela inicial.
void apresentacao()
{
    espera = true;
    placar.exibir = false;
    bnave = false;
    musica = Mix_LoadMUS("audio/apresentacao.mp3");
    Mix_PlayMusic(musica, -1);
    SDL_Delay(4000);
    superficie_fundo = SDL_LoadBMP("graphic/inicio/apresentacao.bmp");
    while (espera)
    {
        SDL_Delay(delay);
    }
    Mix_HaltMusic();
    muda = true;
    fase = 1;
}


// Funcao do final do jogo.
void finale()
{
    espera = true;
    bnave = false;
    musica = Mix_LoadMUS("audio/final.mp3");
    Mix_PlayMusic(musica, -1);
    superficie_fundo = SDL_LoadBMP("graphic/final/final_1.bmp");
    SDL_Delay(5000);
    superficie_fundo = SDL_LoadBMP("graphic/final/final_2.bmp");
    SDL_Delay(5000);
    superficie_fundo = SDL_LoadBMP("graphic/final/final_3.bmp");
    while(espera)
    {
        SDL_Delay(delay);
    }
    Mix_HaltMusic();
    superficie_fundo = SDL_LoadBMP("graphic/inicio/apresentacao.bmp");
    muda = true;
    fase = 0;
    placar.quantos = 0;
}

// Essa funcao sera usada para exibir informacoes no console.
void debug_console()
{
    while (bdebug)
    {
        // Posicao da nave e tempo de jogo.
        printf("Pontos: %i | Nave X: %i Nave Y: %i | Tempo: %i \n", placar.quantos, retangulo.x, retangulo.y, relogio);

        // Delay de thread.
        SDL_Delay(delay);
    }
}

// Main do programa.
int main(int argc, char* args[])
{
    if (carrega_sdl())
    {
        if (carrega_janela())
        {
            if (carrega_midia())
            {
                // Tamanho da nave.
                retangulo.h = 50;
                retangulo.w = 50;

                // Local inicial da nave
                retangulo.x = 378;
                retangulo.y = 524;

                // Tamanho do disparo.
                tiros.ret_disparo1.h = 22;
                tiros.ret_disparo1.w = 15;

                // Flag do LOOP.
                bool quit = false;

                // Exibicao de pontos
                placar.exibir = false;

                placar.ret_ponto1.h = 32;
                placar.ret_ponto1.w = 32;
                placar.ret_ponto1.x = 768;
                placar.ret_ponto1.y = 0;

                placar.ret_ponto2.h = 32;
                placar.ret_ponto2.w = 32;
                placar.ret_ponto2.x = 736;
                placar.ret_ponto2.y = 0;

                placar.ret_ponto3.h = 32;
                placar.ret_ponto3.w = 32;
                placar.ret_ponto3.x = 704;
                placar.ret_ponto3.y = 0;

                // Event handler do SDL.
                SDL_Event e;

                // Thread do Debug.
                tdebug = SDL_CreateThread(debug_console, "debug", (void *)NULL);

                // Thread de desenho dos graficos.
                tdesenha = SDL_CreateThread(desenha, "desenho", (void *)NULL);

                // Thread controle do disparo.
                tcontrole = SDL_CreateThread(controle_tiro, "controle", (void *)NULL);

                // Thread inicial de apresentacao.
                tfase = SDL_CreateThread(apresentacao, "apresentacao", (void *)NULL);

                // LOOP do Jogo.
                while(!quit)
                {
                    // Tempo de alinhamento com as threads.
                    tempo();

                    if (muda)
                    {
                        SDL_DetachThread(tfase);
                        switch(fase)
                        {
                            // Tela inicial.
                            case 0:
                                {
                                    muda = false;
                                    tfase = SDL_CreateThread(apresentacao, "apresentacao", (void *)NULL);
                                    break;
                                }
                            // Fases
                            case 1:
                            case 2:
                            case 3:
                                {
                                    muda = false;
                                    tfase = SDL_CreateThread(ffase, "ffase", (void *)NULL);
                                    break;
                                }
                            // Final
                            case 4:
                                {

                                    muda = false;
                                    tfase = SDL_CreateThread(finale, "finale", (void *)NULL);
                                    break;
                                }
                        }
                    }

                    // Se o jogador pressionar uma tecla.
                    if(e.type == SDL_KEYDOWN)
                    {
                        // Switch/Case da tecla pressionada, cuja informacao esta na variavel "e".
                        switch(e.key.keysym.sym)
                        {
                            // Seta para cima.
                            case SDLK_KP_8:
                            case SDLK_UP:
                            {
                                // Subtrai posicao Y atual do player.
                                retangulo.y = retangulo.y - passo;
                                break;
                            }

                            // Seta para cima+esquerda.
                            case SDLK_KP_7:
                            case SDLK_HOME:
                            {
                                // Subtrai posicao X/Y atual do player.
                                retangulo.x = retangulo.x - passo;
                                retangulo.y = retangulo.y - passo;
                                break;
                            }

                            // Seta para esquerda.
                            case SDLK_KP_4:
                            case SDLK_LEFT:
                            {
                                // Subtrai posicao X atual do player.
                                retangulo.x = retangulo.x - passo;
                                break;
                            }

                            // Seta para esquerda+baixo.
                            case SDLK_KP_1:
                            case SDLK_END:
                            {
                                // Subtrai posicao X atual do player.
                                retangulo.x = retangulo.x - passo;
								// Soma posicao Y atual do player.
                                retangulo.y = retangulo.y + passo;
                                break;
                            }

                            // Seta para baixo.
                            case SDLK_KP_2:
                            case SDLK_DOWN:
                            {
                                // Soma posicao Y atual do player.
                                retangulo.y = retangulo.y + passo;
                                break;
                            }

                            // Seta para baixo+frente.
                            case SDLK_KP_3:
                            case SDLK_PAGEDOWN:
                            {
								// Soma posicao X/Y atual do player.
                                retangulo.x = retangulo.x + passo;
                                retangulo.y = retangulo.y + passo;
                                break;
                            }

                            // Seta para direita.
                            case SDLK_KP_6:
                            case SDLK_RIGHT:
                            {
                                // Soma posicao X atual do player.
                                retangulo.x = retangulo.x + passo;
                                break;
                            }

                            // Seta para direita+cima.
                            case SDLK_KP_9:
                            case SDLK_PAGEUP:
                            {
                                // Soma posicao X atual do player.
								// Subtrai posicao Y atual do player.
                                retangulo.x = retangulo.x + passo;
                                retangulo.y = retangulo.y - passo;
                                break;
                            }
                            // Botao de disparo.
                            case SDLK_LCTRL: // Control esquerdo.
                            case SDLK_RCTRL: // Control direito.
                            case SDLK_5:
                            {

                                if (espera || ((fase == 0)||(fase == 4)) )
                                {
                                    espera = false;
                                }
                                else
                                {
                                    if (bnave)
                                    {
                                        Mix_PlayChannel(-1, sdisparo, 0);
                                        tdisparo = SDL_CreateThread(atirar, "disparo", (void *)NULL);
                                    }
                                }
                                break;
                            }

                            // ESC - sai, por enquanto.
                            case SDLK_ESCAPE:
                                {
                                    quit = true;
                                }

                        } // "Fim switch/case".
                    } // Fim "if".

                    // Limita posicao da nave ao cenario.
                    limita_posicao();

                    // Enquanto o PollEvent nao for 0.
                    while(SDL_PollEvent(&e) != 0)
                    {
                        // Verifica se o usuario selecionou sair.
                        if(e.type == SDL_QUIT)
                        {
                            // Variavel quit se torna verdadeira, possibilitando sair do LOOP.
                            quit = true;
                        }
                    }

                } // Fim "while(!quit)".
            }
            else
            {
                // Nao carregou imagens.
                return 3;
            }
        }
        else
        {
            // Nao carregou janela.
            return 2;
        }
    }
    else
    {
        // Nao carregou SDL.
        return 1;
    }

    // Desliga SDL para saida do programa.
    fechamento();

    // Retorno de sucesso.
	return 0;

} // Fim de tudo.

