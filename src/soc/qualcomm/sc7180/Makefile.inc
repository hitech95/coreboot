
ifeq ($(CONFIG_SOC_QUALCOMM_SC7180),y)

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c
bootblock-y += timer.c
bootblock-y += spi.c

################################################################################
verstage-y += timer.c
verstage-y += spi.c

################################################################################
romstage-y += cbmem.c
romstage-y += timer.c
romstage-y += ../common/qclib.c
romstage-y += qclib.c
romstage-y += ../common/mmu.c
romstage-y += mmu.c
romstage-y += spi.c

################################################################################
ramstage-y += soc.c
ramstage-y += cbmem.c
ramstage-y += timer.c
ramstage-y += spi.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/sc7180/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

################################################################################

endif