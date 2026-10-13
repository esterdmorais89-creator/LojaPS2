#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <libpad.h>
#include <loadfile.h>

// Espaço de memória obrigatório para o comando funcionar
static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    // 1. Inicializa o sistema base
    SifInitRpc(0);

    // 2. Carrega os drivers do comando (diretamente da BIOS da PS2)
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    // 3. Prepara o ecrã
    init_scr();
    scr_clear();

    // 4. Liga o comando na Porta 1
    padInit(0);
    padPortOpen(0, 0, padBuf);

    scr_printf("==================================================\n");
    scr_printf("                 GOD STIX STORE                   \n");
    scr_printf("==================================================\n\n");

    scr_printf(" Escolha o seu idioma / Choose your language:\n\n");
    scr_printf(" Pressione [X] para Portugues\n");
    scr_printf(" Pressione [O] para English\n");

    struct padButtonStatus buttons;
    int new_pad;

    // 5. Loop que fica à espera que carregues num botão
    while (1)
    {
        int state = padGetState(0, 0);

        // Verifica se o comando está pronto para ser lido
        if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1)
        {
            padRead(0, 0, &buttons);
            new_pad = 0xffff ^ buttons.btns; // Inverte os bits para facilitar a leitura

            if (new_pad & PAD_CROSS)
            { // Se carregar no X
                scr_printf("\n  [+] Idioma definido para: PORTUGUES\n");
                break; // Sai deste loop para ir para a próxima etapa
            }
            if (new_pad & PAD_CIRCLE)
            { // Se carregar na Bolinha
                scr_printf("\n  [+] Language set to: ENGLISH\n");
                break; // Sai deste loop para ir para a próxima etapa
            }
        }
    }

    // O código só chega aqui depois de escolheres o idioma!
    scr_printf("\n A preparar o menu de cores...\n");

    // Loop infinito final para segurar o ecrã
    while (1)
    {
    }

    return 0;
}