(* ::Package:: *)

BeginPackage["flame`"];
$flamepath = FileNameJoin[{$UserBaseDirectory, "Applications/flame"}];
$flame = FileNameJoin[{$flamepath, "flame"}];
(* Notebook is in the same directory as flame executable *)
lamp[enable_Integer]:= Module[{p,o,t,a},
  a = If[enable==1,"-h","-l"];
  p = StartProcess[{FileNameJoin[{$flamepath,"flame"}],a}];
  (* Something will be returned if there is an error, so don't remove this *)
  t = ReadString[p,EndOfBuffer];
  o = "";
  While[t=!=EndOfFile,
    o = StringJoin[o,t];
    t = ReadString[p,EndOfBuffer];
  ];
]
	
collect[int_Integer,avg_Integer:1]:= Module[{p,o,t},
  p = StartProcess[{$flame,"-ci",ToString@int,"-a",ToString@avg}];
  (* Something will be returned if there is an error, so don't remove this *)
  t = ReadString[p,EndOfBuffer];
  o = "";
  While[t=!=EndOfFile,
    o = StringJoin[o,t];
    t = ReadString[p,EndOfBuffer];
  ];
  (* Returning only data over 349 nm due to lamp limitations  *)
  Select[
    ImportString[o,"TSV"],
    First@#>349&
  ]
]

process[s_,b_,d_]:=Module[{},
  Transpose[{
    s[[All,1]],
    Log10[
    (b[[All,2]]-d[[All,2]])/(s[[All,2]]-d[[All,2]])]
  }]
];

Clear[flamePlot];
flamePlot[s_,b_,d_]:=Module[{},
ListLinePlot[Quiet@process[s,b,d],Axes->False,
Frame->{True,True,False,False},FrameLabel->{"Wavelength (nm)","Absorbance"},ImageSize->300,
PlotLabel->Dynamic["\[Lambda]: "<>ToString[Round[#[[1]],0.1]]<> " A: "<>ToString[Round[#[[2]],0.001]]&@MousePosition[{"Graphics",Graphics},{0,0}]]
]
];
Clear[flameInterface];
Options[flameInterface] = {"Magnification" -> 1.2};
flameInterface[OptionsPattern[]] := CreatePalette[DynamicModule[{out="Nothing Yet",fs = 18,int = 10000,avg=1,dk=Null,sm=Null,bg=Null},

Row[{
Column[{
Row[{Style["Lamp: ",fs,Bold],Button[Style["ON",fs],lamp[1]],Button[Style["OFF",fs], lamp[0]]},"   "],
Style["Integration time",Bold,0.5 fs],
Slider[Dynamic@int,{1000,150000,10},Appearance->"Labeled"],
Style["# Scans",Bold,0.5 fs],
Slider[Dynamic@avg,{1,10,1},Appearance->"Labeled"],
Style["Spectra",fs,Bold],
Grid[{{Button[Style["Dark",fs],dk = collect[int,avg]],
Dynamic@If[Head[dk]===List,"\[Checkmark]","\[Chi]"]
},{Button[Style["Background",fs],bg = collect[int,avg]],
Dynamic@If[Head[bg]===List,"\[Checkmark]","\[Chi]"]
},{
Button[Style["Sample",fs],(
sm = collect[int,avg];
out = If[And[Head@dk===List,Head@bg===List],flamePlot[sm,bg,dk],"Nothing Yet"];
)],
Dynamic@If[Head[sm]===List,"\[Checkmark]","\[Chi]"]}
}],
Style["Other",fs,Bold],
Button[Style["Save",fs],Export[SystemDialogInput["FileSave",".csv"],Quiet@process[sm,bg,dk]],Method->"Queued"],
Button[Style["Close",fs],(
lamp[0];NotebookClose/@Notebooks["Flame Interface"]
)]
},Alignment->Center],
Dynamic@out},"\t"]
],
WindowTitle->"Flame Interface",
WindowFloating->False,
WindowClickSelect->True,
Saveable->False,
Magnification->OptionValue["Magnification"],
WindowSize->{Scaled[1],Scaled[1]}
]

EndPackage[];
