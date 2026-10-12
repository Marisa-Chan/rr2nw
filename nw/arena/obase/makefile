!include ..\..\make.inc

!ifneq DEBUG 0
T = obj\wd
!else
T = obj\wr
!endif


OTAR  = $(T)\bullet.obj $(T)\cannon.obj $(T)\comander.obj 
OTAR += $(T)\dcross.obj 
OTAR += $(T)\group.obj $(T)\group_2.obj
OTAR += $(T)\publish.obj $(T)\spark.obj
OTAR += $(T)\tank.obj $(T)\tank_2.obj $(T)\tank_3.obj $(T)\nearai.obj
OTAR += $(T)\t_dynobj.obj
OTAR += $(T)\skin.obj $(T)\dynobj.obj $(T)\dynspr.obj $(T)\dynpart.obj $(T)\dynpartln.obj
OTAR += $(T)\skinspr.obj
OTAR += $(T)\soundobj.obj $(T)\wavobj.obj
OTAR += $(T)\fountain.obj $(T)\smoke.obj $(T)\smoker.obj
OTAR += $(T)\vehicle.obj  $(T)\vh_dyn.obj $(T)\vh_vessel.obj $(T)\vs_zav.obj
OTAR += $(T)\explosion.obj
OTAR += $(T)\bird.obj $(T)\taxi.obj   $(T)\people.obj $(T)\pdy_obj.obj
OTAR += $(T)\route.obj $(T)\route_i.obj
OTAR += $(T)\farter.obj
OTAR += $(T)\staticinit.obj $(T)\staticobj.obj
OTAR += $(T)\menu.obj
OTAR += $(T)\dyncorona.obj
OTAR += $(T)\lamp.obj
OTAR += $(T)\dmap.obj
OTAR += $(T)\player.obj
OTAR += $(T)\corpse.obj
OTAR += $(T)\orphan.obj
OTAR += $(T)\recrcen.obj
OTAR += $(T)\artefact.obj $(T)\animpeop.obj $(T)\portal.obj
OTAR += $(T)\howitzer.obj

obasew.lib: $(OTAR)
	wlib -p=2048 obasew.lib $(OTAR)


TARGETS = obasew.lib

$(BUILD)




