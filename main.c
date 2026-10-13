#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>

int main(int argc, char *argv[])
{
    // 1. Inicializa os sistemas base da PS2
    SifInitRpc(0);

    // 2. Prepara o ecrã da televisão
    init_scr();
    scr_clear();

    // 3. Desenha o cabeçalho da loja
    scr_printf("==================================================\n");
    scr_printf("                 GOD STIX STORE                   \n");
    scr_printf("==================================================\n\n");

    // 4. Mostra o estado do sistema
    scr_printf("  [+] Interface visual carregada com sucesso!\n");
    scr_printf("  [+] Versao: 0.1 Alpha\n\n");

    scr_printf("  A aguardar ligacao aos servidores...\n");

    // 5. Loop infinito para manter a imagem no ecrã
    while (1)
    {
        // O programa fica aqui a correr continuamente
    }

    return 0;
}