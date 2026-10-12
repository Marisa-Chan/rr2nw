func void main_CreateFountain()
{
    s_AddClassTable( "Fountain",10 );
    CreateFountain("Fount.0","Fount.Attr.Water",[3740.876, 100.751, -3692.465]);
//    CreateFountain("Fount.1","Fount.Attr.Water",[210+10,25,-244]);
//    CreateFountain("Fount.2","Fount.Attr.Volcano",[2881.34,228.7,-2677.5]);
    CreateFountain("Fount.2", "Fount.Attr.Blob",[1957.727,10.077, -2671.745-2]);
    CreateFountain("Fount.2b","Fount.Attr.Blob",[1957.727+1,10.077, -2671.745+2]);
    CreateFountain("Fount.3b","Fount.Attr.Blob",[1957.727+4,10.077, -2671.745]);
    CreateFountain("Fount.4b","Fount.Attr.Blob",[1957.727+7,10.077, -2671.745+3]);
    CreateFountain("Fount.5b","Fount.Attr.Blob",[1957.727+12,10.077, -2671.745]);
}

