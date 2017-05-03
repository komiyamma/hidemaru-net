declare var clr: any;

declare namespace host {
    function lib(libname: string): any;
    function lib(libns: any, libname: string): any;
}

type States = (f: any)=>void;
declare namespace hm {
    namespace Macro {
        function Eval(expression:States | string );
        let Var: Map<string, any>;
    }
    namespace Edit {
        let TotalText: string;
        let SelectedText: string;
    }
}

declare interface IJapanesePhonetic {
    DisplayText: string;
    IsPhraseStart: boolean;
    YomiText: string;
}

declare class HmJapanesePhoneticAnalyzer {
    public GetJapanesePhoneticAnalyzeDataList(target_data: string): IJapanesePhonetic[];
}


