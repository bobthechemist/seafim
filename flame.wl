(* ::Package:: *)

(* ::Input:: *)
(*(* Notebook is in the same directory as flame executable *)*)
(*lamp[enable_Integer]:= Module[{p,o,t,a},*)
(*a = If[enable==1,"-h","-l"];*)
(*p = StartProcess[{"./flame",a}];*)
(*(* Something will be returned if there is an error, so don't remove this *)*)
(*t = ReadString[p,EndOfBuffer];*)
(*o = "";*)
(*While[t=!=EndOfFile,*)
(*o = StringJoin[o,t];*)
(*t = ReadString[p,EndOfBuffer];*)
(*];*)
(*]*)
(*collect[int_Integer]:= Module[{p,o,t},*)
(*p = StartProcess[{"./flame","-ci",ToString[int]}];*)
(*(* Something will be returned if there is an error, so don't remove this *)*)
(*t = ReadString[p,EndOfBuffer];*)
(*o = "";*)
(*While[t=!=EndOfFile,*)
(*o = StringJoin[o,t];*)
(*t = ReadString[p,EndOfBuffer];*)
(*];*)
(*ImportString[o,"TSV"]*)
(*]*)
(*process[s_,b_,d_]:=Module[{},*)
(*Transpose[{*)
(*s[[All,1]],*)
(*Log10[*)
(*(b[[All,2]]-d[[All,2]])/(s[[All,2]]-d[[All,2]])]*)
(*}]*)
(*];*)
