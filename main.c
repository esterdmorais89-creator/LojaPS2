#include <tamtypes.h>
#include <sifrpc.h>
#include <gsKit.h>
#include <dmaKit.h>

int main(int argc, char *argv[])
{
    // 1. Inicializa o sistema base
    SifInitRpc(0);

    // 2. Inicializa o motor de transferência de dados (DMA)
    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    // 3. Inicializa a Placa Gráfica da PS2 (GS)
    GSGLOBAL *gsGlobal = gsKit_init_global();

    // Configura a resolução para a TV
    gsGlobal->Mode = gsKit_check_rom();
    if (gsGlobal->Mode == GS_MODE_PAL)
    {
        gsGlobal->Height = 512;
    }
    else
    {
        gsGlobal->Height = 448;
    }

    gsKit_init_screen(gsGlobal);
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    // 4. Define as nossas cores (R, G, B, Transparência)
    u64 CorFundo = GS_SETREG_RGBAQ(20, 30, 50, 0x00, 0x00);   // Azul Escuro (Fundo)
    u64 CorHeader = GS_SETREG_RGBAQ(200, 30, 30, 0x00, 0x00); // Vermelho (Barra do topo)
    u64 CorMenu = GS_SETREG_RGBAQ(40, 60, 90, 0x00, 0x00);    // Azul Claro (Caixa dos jogos)

    // 5. Loop principal de desenho gráfico
    while (1)
    {
        // Limpa o ecrã com a cor de fundo
        gsKit_clear(gsGlobal, CorFundo);

        // Desenha a barra vermelha no topo (X inicial, Y inicial, X final, Y final, Z, Cor)
        gsKit_prim_sprite(gsGlobal, 0, 0, 640, 70, 0, CorHeader);

        // Desenha a caixa central onde vai ficar a lista de jogos
        gsKit_prim_sprite(gsGlobal, 80, 100, 560, 350, 0, CorMenu);

        // Envia o desenho para a TV/Emulador
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);
    }

    return 0;
}