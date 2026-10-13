#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <libpad.h>
#include <loadfile.h>

// Códigos de cor
#define COR_VERMELHA "\033[31m"
#define COR_VERDE "\033[32m"
#define COR_AMARELA "\033[33m"
#define COR_RESET "\033[0m"

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    init_scr();
    scr_clear();
    padInit(0);
    padPortOpen(0, 0, padBuf);

    scr_printf("==================================================\n");
    scr_printf("                 GOD STIX STORE                   \n");
    scr_printf("==================================================\n\n");

    scr_printf(" Escolha o seu idioma / Choose your language:\n\n");
    scr_printf(" [X] Portugues\n");
    scr_printf(" [O] English\n");

    struct padButtonStatus buttons;
    int new_pad;
    int idioma_escolhido = 0; // 1 = PT, 2 = EN

    // 1. Menu de Idioma
    while (1)
    {
        int state = padGetState(0, 0);
        if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1)
        {
            padRead(0, 0, &buttons);
            new_pad = 0xffff ^ buttons.btns;

            if (new_pad & PAD_CROSS)
            {
                scr_printf("\n  [+] Idioma: PORTUGUES\n");
                idioma_escolhido = 1;
                break;
            }
            if (new_pad & PAD_CIRCLE)
            {
                scr_printf("\n  [+] Language: ENGLISH\n");
                idioma_escolhido = 2;
                break;
            }
        }
    }

    // Pausa rápida para não ler o botão duas vezes seguidas acidentalmente
    int delay = 10000000;
    while (delay--)
        asm("nop");

    // 2. Menu de Cores adaptado ao idioma
    scr_printf("\n==================================================\n");
    if (idioma_escolhido == 1)
    {
        scr_printf(" Escolha a cor do sistema:\n\n");
        scr_printf(" [Triangulo] Verde\n [Quadrado] Amarelo\n [R1] Vermelho\n");
    }
    else
    {
        scr_printf(" Choose system color:\n\n");
        scr_printf(" [Triangle] Green\n [Square] Yellow\n [R1] Red\n");
    }

    // 3. Aguarda a escolha da cor
    while (1)
    {
        int state = padGetState(0, 0);
        if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1)
        {
            padRead(0, 0, &buttons);
            new_pad = 0xffff ^ buttons.btns;

            if (new_pad & PAD_TRIANGLE)
            {
                scr_printf(COR_VERDE "\n  [+] Loja Iniciada! (Verde)\n" COR_RESET);
                break;
            }
            if (new_pad & PAD_SQUARE)
            {
                scr_printf(COR_AMARELA "\n  [+] Loja Iniciada! (Amarelo)\n" COR_RESET);
                break;
            }
            if (new_pad & PAD_R1)
            {
                scr_printf(COR_VERMELHA "\n  [+] Loja Iniciada! (Vermelho)\n" COR_RESET);
                break;
            }
        }
    }

    // Loop final
    while (1)
    {
    }

    return 0;
}