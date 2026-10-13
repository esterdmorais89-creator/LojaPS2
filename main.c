#include <tamtypes.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <gsKit.h>
#include <dmaKit.h>

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    padInit(0);
    padPortOpen(0, 0, padBuf);

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    GSGLOBAL *gsGlobal = gsKit_init_global();
    gsGlobal->Mode = gsKit_check_rom();
    gsGlobal->Height = (gsGlobal->Mode == GS_MODE_PAL) ? 512 : 448;

    gsKit_init_screen(gsGlobal);
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    u64 CorFundo = GS_SETREG_RGBAQ(20, 30, 50, 0x00, 0x00);
    u64 CorHeader = GS_SETREG_RGBAQ(200, 30, 30, 0x00, 0x00);
    u64 CorMenu = GS_SETREG_RGBAQ(40, 60, 90, 0x00, 0x00);
    u64 CorCursor = GS_SETREG_RGBAQ(220, 180, 40, 0x00, 0x00);

    struct padButtonStatus buttons;
    int new_pad;
    int cursor = 0;
    int total_jogos = 5;

    while (1)
    {
        gsKit_clear(gsGlobal, CorFundo);

        gsKit_prim_sprite(gsGlobal, 0, 0, 640, 70, 0, CorHeader);
        gsKit_prim_sprite(gsGlobal, 80, 100, 560, 350, 0, CorMenu);

        int y_inicial = 115 + (cursor * 45);
        gsKit_prim_sprite(gsGlobal, 90, y_inicial, 550, y_inicial + 35, 0, CorCursor);

        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);

        int state = padGetState(0, 0);
        if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1)
        {
            padRead(0, 0, &buttons);
            new_pad = 0xffff ^ buttons.btns;

            if (new_pad & PAD_UP)
            {
                cursor--;
                if (cursor < 0)
                    cursor = total_jogos - 1;
            }
            if (new_pad & PAD_DOWN)
            {
                cursor++;
                if (cursor >= total_jogos)
                    cursor = 0;
            }
        }

        for (int delay = 0; delay < 3000000; delay++)
            asm("nop");
    }

    return 0;
}