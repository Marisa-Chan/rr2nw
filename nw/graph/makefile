!include ..\make.inc

TARGETS = $(O)\clip2da.obj $(O)\drawpoly.obj $(O)\sprite.obj $(O)\drawd3d.obj $(O)\graph.obj
TARGETS += $(O)\polygon.obj $(O)\texture.obj $(O)\texturel.obj $(O)\hazea.obj $(O)\gouraud.obj $(O)\bump.obj
TARGETS += $(O)\light1.obj $(O)\light.obj $(O)\alpha.obj $(O)\spritel.obj $(O)\spritelh.obj $(O)\diser.obj
TARGETS += $(O)\muldiv.obj $(O)\sd1_epal.obj $(O)\ctrlbrk.obj $(O)\sobject.obj $(O)\fixedfnt.obj $(O)\fixfnta.obj $(O)\ray.obj $(O)\map.obj
TARGETS += $(O)\spr_toon.obj $(O)\image.obj $(O)\particle.obj $(O)\particla.obj $(O)\alphaspr.obj $(O)\panela.obj $(O)\panel.obj $(O)\2dgraph.obj

$(BUILD)

3dgraph.lib: $(TARGETS)
        del 3dgraph.lib
        wlib 3dgraph +$(O)\clip2da.obj +$(O)\drawpoly.obj +$(O)\sprite.obj +$(O)\drawd3d.obj +$(O)\graph.obj +$(O)\polygon.obj +$(O)\texture.obj +$(O)\texturel.obj +$(O)\hazea.obj +$(O)\gouraud.obj +$(O)\bump.obj +$(O)\light1.obj +$(O)\light.obj +$(O)\alpha.obj +$(O)\spritel.obj +$(O)\spritelh.obj +$(O)\muldiv.obj +$(O)\sd1_epal.obj +$(O)\ctrlbrk.obj +$(O)\sobject.obj +$(O)\fixedfnt.obj +$(O)\fixfnta.obj +$(O)\ray.obj +$(O)\spr_toon.obj +$(O)\image.obj +$(O)\particle.obj +$(O)\particla.obj +$(O)\alphaspr.obj +$(O)\panela.obj +$(O)\panel.obj