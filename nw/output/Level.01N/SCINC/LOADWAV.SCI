func void LoadAllWaves(int classTableID)
{

    LoadWAV( classTableID, "wav.Ambient" , "Ambient.wav",
                   15,//float  minFront,
                   15,//float  minBack,
                   100,//float  maxFront,
                   100,//float  maxBack,
                   1.0 //float  intensity
               );


    LoadWAV( classTableID, "wav.Shoot.Gun1" , "gun1.wav",
                   50,//float  minFront,
                   50,//float  minBack,
                   250,//float  maxFront,
                   250,//float  maxBack,
                   1.0  //float  intensity
               );

    LoadWAV( classTableID, "wav.Shoot.Gun2" , "gun2.wav",
                   50,//float  minFront,
                   50,//float  minBack,
                   250,//float  maxFront,
                   250,//float  maxBack,
                   1.0  //float  intensity
               );

    LoadWAV( classTableID, "wav.Explosion", "Explosion1.wav",
                   100,//float  minFront,
                   100,//float  minBack,
                   1550,//float  maxFront,
                   1550,//float  maxBack,
                   1.0  //float  intensity
               );

    LoadWAV( classTableID, "wav.Spaces", "Spaces.wav",
                   100,//float  minFront,
                   100,//float  minBack,
                   1550,//float  maxFront,
                   1550,//float  maxBack,
                   1.0  //float  intensity
               );


}
