xof 0303txt 0032
template FVFData {
 <b6e70a0e-8ef9-4e83-94ad-ecc8b0c04897>
 DWORD dwFVF;
 DWORD nDWords;
 array DWORD data[nDWords];
}

template EffectInstance {
 <e331f7e4-0559-4cc2-8e99-1cec1657928f>
 STRING EffectFilename;
 [...]
}

template EffectParamFloats {
 <3014b9a0-62f5-478c-9b86-e4ac9f4e418b>
 STRING ParamName;
 DWORD nFloats;
 array FLOAT Floats[nFloats];
}

template EffectParamString {
 <1dbc4c88-94c1-46ee-9076-2c28818c9481>
 STRING ParamName;
 STRING Value;
}

template EffectParamDWord {
 <e13963bc-ae51-4c5d-b00f-cfa3a9d97ce5>
 STRING ParamName;
 DWORD Value;
}


Material Material__42 {
 1.000000;1.000000;1.000000;1.000000;;
 3.200000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "res/texture/AshenNatural.png";
 }
}

Frame cattailgrass3 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,-0.000000,1.000000,0.000000,0.000000,-1.000000,-0.000000,0.000000,-2.182830,0.000000,8.064450,1.000000;;
 }

 Frame {
  

  FrameTransformMatrix {
   1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,-0.000000,1.000000;;
  }
 }
}

Frame AshenCatTail0 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh  {
  124;
  7.431140;71.076401;24.521900;,
  7.235510;71.091904;21.715700;,
  22.744400;239.352005;39.961300;,
  -5.323020;0.177490;6.020030;,
  7.042530;71.107101;18.947500;,
  -4.813970;0.137430;13.322500;,
  9.724610;70.896004;22.970301;,
  1.236120;-0.338710;9.229040;,
  9.563110;70.908699;20.180700;,
  12.049500;70.712898;21.397100;,
  -5.323020;0.177490;6.020030;,
  22.744400;239.352005;39.961300;,
  9.724610;70.896004;22.970301;,
  7.431140;71.076401;24.521900;,
  7.235510;71.091904;21.715700;,
  7.431140;71.076401;24.521900;,
  7.042530;71.107101;18.947500;,
  22.744400;239.352005;39.961300;,
  17.002300;126.708000;33.085800;,
  10.819400;127.064003;31.898001;,
  11.968400;183.233002;37.862701;,
  19.944401;182.774994;39.394901;,
  20.066900;127.064003;27.585800;,
  23.897600;183.233002;32.299900;,
  15.777700;127.638000;22.998800;,
  18.364700;183.975006;26.382900;,
  10.062600;127.638000;25.663900;,
  10.992000;183.975006;29.820801;,
  10.819400;127.064003;31.898001;,
  11.968400;183.233002;37.862701;,
  -17.497400;107.349998;-11.100700;,
  -11.368900;107.438004;-12.583900;,
  -12.434400;163.707993;-17.535200;,
  -20.340200;163.593994;-15.621800;,
  -17.989201;107.726997;-4.825300;,
  -20.974701;164.080994;-7.526470;,
  -12.164600;108.047997;-2.430020;,
  -13.461000;164.494995;-4.436560;,
  -8.073100;107.870003;-7.225160;,
  -8.182920;164.264008;-10.622200;,
  -11.368900;107.438004;-12.583900;,
  -12.434400;163.707993;-17.535200;,
  14.645700;122.114998;28.031900;,
  10.819400;127.064003;31.898001;,
  17.002300;126.708000;33.085800;,
  20.066900;127.064003;27.585800;,
  15.777700;127.638000;22.998800;,
  10.062600;127.638000;25.663900;,
  17.377800;189.253006;33.890800;,
  23.897600;183.233002;32.299900;,
  19.944401;182.774994;39.394901;,
  11.968400;183.233002;37.862701;,
  10.992000;183.975006;29.820801;,
  18.364700;183.975006;26.382900;,
  -13.461600;102.575996;-7.479310;,
  -8.073100;107.870003;-7.225160;,
  -11.368900;107.438004;-12.583900;,
  -17.497400;107.349998;-11.100700;,
  -17.989201;107.726997;-4.825300;,
  -12.164600;108.047997;-2.430020;,
  -15.444100;169.865005;-11.677800;,
  -20.974701;164.080994;-7.526470;,
  -20.340200;163.593994;-15.621800;,
  -12.434400;163.707993;-17.535200;,
  -8.182920;164.264008;-10.622200;,
  -13.461000;164.494995;-4.436560;,
  -5.915110;0.000000;3.220940;,
  1.405180;0.000000;3.220940;,
  -10.431500;64.037201;-5.611380;,
  -8.320260;64.037201;-5.611380;,
  -11.158300;64.037201;-3.782860;,
  -12.213900;64.037201;-5.611380;,
  -2.254950;0.000000;9.560500;,
  -17.487101;217.391998;-14.652700;,
  -9.211460;64.037201;-4.067790;,
  -17.487101;217.391998;-14.652700;,
  -2.254950;0.000000;9.560500;,
  -10.267100;64.037201;-2.239270;,
  -10.431500;64.037201;-5.611380;,
  -8.320260;64.037201;-5.611380;,
  -9.211460;64.037201;-4.067790;,
  -8.320260;64.037201;-5.611380;,
  -52.239601;-3.749260;11.047500;,
  -53.148899;94.337898;-9.781570;,
  47.943501;-3.749260;6.673570;,
  47.034000;94.337898;-14.155500;,
  12.431500;94.337898;-43.920700;,
  28.972200;-3.749260;-31.228500;,
  -48.614101;94.337898;35.635601;,
  -32.073299;-3.749260;48.327702;,
  45.845001;94.337898;32.749500;,
  30.110001;-3.749260;46.427799;,
  -19.943501;94.337898;-42.931400;,
  -35.678600;-3.749270;-29.253201;,
  -22.234800;94.337898;-42.074699;,
  -1.385890;-3.749260;-42.074699;,
  -22.234800;94.337898;58.203701;,
  -1.385890;-3.749260;58.203701;,
  -5.323020;0.177490;6.020030;,
  9.563110;70.908699;20.180700;,
  7.042530;71.107101;18.947500;,
  9.563110;70.908699;20.180700;,
  12.049500;70.712898;21.397100;,
  22.744400;239.352005;39.961300;,
  1.236120;-0.338710;9.229040;,
  9.724610;70.896004;22.970301;,
  12.049500;70.712898;21.397100;,
  7.235510;71.091904;21.715700;,
  7.042530;71.107101;18.947500;,
  22.744400;239.352005;39.961300;,
  -2.254950;0.000000;9.560500;,
  -11.158300;64.037201;-3.782860;,
  -10.267100;64.037201;-2.239270;,
  -11.158300;64.037201;-3.782860;,
  -12.213900;64.037201;-5.611380;,
  -17.487101;217.391998;-14.652700;,
  -5.915110;0.000000;3.220940;,
  -10.431500;64.037201;-5.611380;,
  -12.213900;64.037201;-5.611380;,
  -9.211460;64.037201;-4.067790;,
  -10.267100;64.037201;-2.239270;,
  -17.487101;217.391998;-14.652700;,
  -10.267100;64.037201;-2.239270;,
  -17.487101;217.391998;-14.652700;;
  78;
  3;0,2,1;,
  3;1,4,3;,
  3;5,1,3;,
  3;6,0,5;,
  3;7,6,5;,
  3;8,9,7;,
  3;10,8,7;,
  3;9,11,6;,
  3;12,2,13;,
  3;5,15,14;,
  3;16,17,8;,
  3;18,20,19;,
  3;18,21,20;,
  3;22,21,18;,
  3;22,23,21;,
  3;24,23,22;,
  3;24,25,23;,
  3;26,25,24;,
  3;26,27,25;,
  3;28,27,26;,
  3;28,29,27;,
  3;30,32,31;,
  3;30,33,32;,
  3;34,33,30;,
  3;34,35,33;,
  3;36,35,34;,
  3;36,37,35;,
  3;38,37,36;,
  3;38,39,37;,
  3;40,39,38;,
  3;40,41,39;,
  3;42,44,43;,
  3;42,45,44;,
  3;42,46,45;,
  3;42,43,47;,
  3;42,47,46;,
  3;48,50,49;,
  3;48,51,50;,
  3;48,52,51;,
  3;48,53,52;,
  3;48,49,53;,
  3;54,56,55;,
  3;54,57,56;,
  3;54,58,57;,
  3;54,55,59;,
  3;54,59,58;,
  3;60,62,61;,
  3;60,63,62;,
  3;60,64,63;,
  3;60,65,64;,
  3;60,61,65;,
  3;66,68,67;,
  3;68,69,67;,
  3;70,71,66;,
  3;72,70,66;,
  3;71,73,68;,
  3;69,75,74;,
  3;74,77,76;,
  3;67,74,76;,
  3;78,75,79;,
  3;67,81,80;,
  3;82,84,83;,
  3;85,83,84;,
  3;86,88,87;,
  3;89,87,88;,
  3;90,92,91;,
  3;93,91,92;,
  3;94,96,95;,
  3;97,95,96;,
  3;98,100,99;,
  3;101,103,102;,
  3;104,106,105;,
  3;107,109,108;,
  3;110,112,111;,
  3;113,115,114;,
  3;116,118,117;,
  3;119,121,120;,
  3;122,123,70;;

  MeshNormals  {
   68;
   -0.448442;-0.051974;0.892299;,
   -0.071555;0.994647;0.074550;,
   -0.989178;0.129122;0.069667;,
   -0.541961;0.197764;-0.816803;,
   -0.533611;0.240790;-0.810727;,
   -0.452500;-0.062335;0.889583;,
   0.540973;-0.183148;0.820857;,
   0.987540;-0.137049;-0.077344;,
   0.443113;0.079339;-0.892948;,
   0.992692;-0.096340;-0.072672;,
   0.194376;-0.119758;0.973589;,
   -0.552186;-0.076862;0.830171;,
   -0.872253;-0.033966;0.487874;,
   0.620870;-0.119758;0.774712;,
   0.990886;-0.076862;0.110625;,
   0.934407;-0.033966;-0.354585;,
   0.416527;0.035441;-0.908432;,
   -0.044873;0.061952;-0.997070;,
   -0.734956;0.061952;-0.675279;,
   -0.963638;0.035441;-0.264851;,
   -0.585286;-0.094318;-0.805322;,
   0.154698;-0.083689;-0.984411;,
   0.589354;-0.059730;-0.805664;,
   -0.890616;-0.080987;-0.447487;,
   -0.949994;-0.035458;0.310250;,
   -0.704042;-0.003259;0.710151;,
   -0.000755;0.035508;0.999369;,
   0.456581;0.042077;0.888686;,
   0.950614;0.020507;0.309697;,
   0.987312;-0.007632;-0.158610;,
   -0.034429;-0.996676;-0.073834;,
   0.268709;-0.771838;0.576248;,
   -0.533121;-0.736899;0.415646;,
   0.661087;-0.736899;-0.141222;,
   0.104277;-0.681435;-0.724411;,
   -0.621960;-0.681435;-0.385762;,
   0.045269;0.994247;0.097079;,
   0.308330;0.683906;0.661215;,
   0.677243;0.735709;-0.008605;,
   -0.441915;0.735709;0.513267;,
   -0.525370;0.817501;-0.235961;,
   0.156945;0.817501;-0.554130;,
   0.018414;-0.998430;0.052908;,
   0.280313;-0.749840;-0.599303;,
   0.695517;-0.712452;0.093106;,
   -0.516688;-0.750014;-0.412932;,
   -0.574626;-0.712513;0.402655;,
   0.175601;-0.689752;0.702429;,
   -0.038005;0.996853;-0.069566;,
   -0.524410;0.707454;-0.473818;,
   -0.580022;0.761734;0.288678;,
   0.221189;0.726753;-0.650312;,
   0.611029;0.791602;-0.003152;,
   0.122557;0.812929;0.569321;,
   -0.859938;-0.151798;-0.487303;,
   0.000000;-0.105599;-0.994409;,
   0.859950;0.060708;-0.506755;,
   0.862724;0.041349;-0.503981;,
   -0.865982;-0.009852;0.499977;,
   -0.863534;-0.105706;-0.493087;,
   -0.016673;0.179171;0.983677;,
   -0.129217;0.991607;0.004297;,
   0.856150;0.150584;0.494300;,
   -0.009786;0.130147;0.991446;,
   0.042666;0.207912;0.977217;,
   0.776017;0.207912;0.595459;,
   -0.738218;0.207912;0.641722;,
   0.978148;0.207912;0.000000;;
   78;
   3;0,1,2;,
   3;2,3,4;,
   3;5,2,4;,
   3;6,0,5;,
   3;7,6,5;,
   3;8,9,7;,
   3;4,8,7;,
   3;9,1,6;,
   3;6,1,0;,
   3;5,0,2;,
   3;3,1,8;,
   3;10,11,12;,
   3;10,13,11;,
   3;14,13,10;,
   3;14,15,13;,
   3;16,15,14;,
   3;16,17,15;,
   3;18,17,16;,
   3;18,19,17;,
   3;12,19,18;,
   3;12,11,19;,
   3;20,21,22;,
   3;20,23,21;,
   3;24,23,20;,
   3;24,25,23;,
   3;26,25,24;,
   3;26,27,25;,
   3;28,27,26;,
   3;28,29,27;,
   3;22,29,28;,
   3;22,21,29;,
   3;30,31,32;,
   3;30,33,31;,
   3;30,34,33;,
   3;30,32,35;,
   3;30,35,34;,
   3;36,37,38;,
   3;36,39,37;,
   3;36,40,39;,
   3;36,41,40;,
   3;36,38,41;,
   3;42,43,44;,
   3;42,45,43;,
   3;42,46,45;,
   3;42,44,47;,
   3;42,47,46;,
   3;48,49,50;,
   3;48,51,49;,
   3;48,52,51;,
   3;48,53,52;,
   3;48,50,53;,
   3;54,55,56;,
   3;55,57,56;,
   3;58,59,54;,
   3;60,58,54;,
   3;59,61,55;,
   3;57,61,62;,
   3;62,63,60;,
   3;56,62,60;,
   3;55,61,57;,
   3;56,57,62;,
   3;64,64,64;,
   3;64,64,64;,
   3;65,65,65;,
   3;65,65,65;,
   3;66,66,66;,
   3;66,66,66;,
   3;67,67,67;,
   3;67,67,67;,
   3;4,3,8;,
   3;8,1,9;,
   3;7,9,6;,
   3;2,1,3;,
   3;60,63,58;,
   3;58,61,59;,
   3;54,59,55;,
   3;62,61,63;,
   3;63,61,58;;
  }

  MeshMaterialList  {
   1;
   78;
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0;
   { Material__42 }
  }

  MeshTextureCoords  {
   124;
   0.123315;0.147542;,
   0.148283;0.147542;,
   0.123315;0.000499;,
   0.172913;0.296602;,
   0.172913;0.147542;,
   0.123315;0.296602;,
   0.102687;0.147542;,
   0.081775;0.296602;,
   0.057145;0.147542;,
   0.081775;0.147542;,
   0.032177;0.296602;,
   0.081775;0.000499;,
   0.081775;0.296602;,
   0.123315;0.296602;,
   0.172913;0.000499;,
   0.123315;0.000499;,
   0.032177;0.147542;,
   0.032177;0.000499;,
   0.038778;0.227232;,
   0.047202;0.227232;,
   0.047202;0.054002;,
   0.038778;0.054002;,
   0.030354;0.227232;,
   0.030354;0.054002;,
   0.023037;0.227232;,
   0.023037;0.054002;,
   0.015613;0.227232;,
   0.015613;0.054002;,
   0.008296;0.227232;,
   0.008296;0.054002;,
   0.038778;0.227232;,
   0.047202;0.227232;,
   0.047202;0.054002;,
   0.038778;0.054002;,
   0.030354;0.227232;,
   0.030354;0.054002;,
   0.023037;0.227232;,
   0.023037;0.054002;,
   0.015613;0.227232;,
   0.015613;0.054002;,
   0.008296;0.227232;,
   0.008296;0.054002;,
   0.043478;0.151602;,
   0.007598;0.140514;,
   0.043478;0.115722;,
   0.079359;0.140514;,
   0.065654;0.180630;,
   0.021303;0.180630;,
   0.043478;0.151602;,
   0.079359;0.140514;,
   0.043478;0.115722;,
   0.007598;0.140514;,
   0.021303;0.180630;,
   0.065654;0.180630;,
   0.043478;0.151602;,
   0.021303;0.180630;,
   0.007598;0.140514;,
   0.043478;0.115722;,
   0.079359;0.140514;,
   0.065654;0.180630;,
   0.043478;0.151602;,
   0.079359;0.140514;,
   0.043478;0.115722;,
   0.007598;0.140514;,
   0.021303;0.180630;,
   0.065654;0.180630;,
   0.081775;0.296602;,
   0.123315;0.296602;,
   0.100790;0.161060;,
   0.123315;0.161060;,
   0.054880;0.161060;,
   0.081775;0.161060;,
   0.032177;0.296602;,
   0.081775;0.000499;,
   0.146019;0.161060;,
   0.123315;0.000499;,
   0.172913;0.296602;,
   0.172913;0.161060;,
   0.081775;0.296602;,
   0.123315;0.296602;,
   0.172913;0.000499;,
   0.123315;0.000499;,
   0.230640;0.999500;,
   0.230641;0.694643;,
   0.000499;0.999500;,
   0.000500;0.694643;,
   0.000500;0.694643;,
   0.000499;0.999500;,
   0.230641;0.694643;,
   0.230640;0.999500;,
   0.000500;0.694643;,
   0.000499;0.999500;,
   0.230641;0.694643;,
   0.230640;0.999500;,
   0.000500;0.694643;,
   0.000499;0.999500;,
   0.230641;0.694643;,
   0.230640;0.999500;,
   0.102687;0.147542;,
   0.123315;0.147542;,
   0.032177;0.000499;,
   0.032177;0.296602;,
   0.148283;0.147542;,
   0.123315;0.147542;,
   0.148283;0.147542;,
   0.172913;0.147542;,
   0.081775;0.000499;,
   0.123315;0.296602;,
   0.172913;0.296602;,
   0.172913;0.000499;,
   0.100790;0.161060;,
   0.123315;0.161060;,
   0.032177;0.000499;,
   0.032177;0.296602;,
   0.146019;0.161060;,
   0.123315;0.161060;,
   0.146019;0.161060;,
   0.172913;0.161060;,
   0.081775;0.000499;,
   0.123315;0.296602;,
   0.172913;0.296602;,
   0.172913;0.000499;,
   0.032177;0.161060;,
   0.032177;0.000499;;
  }
 }
}