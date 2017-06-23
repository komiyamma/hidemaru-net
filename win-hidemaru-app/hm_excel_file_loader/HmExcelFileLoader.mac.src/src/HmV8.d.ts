declare var clr: any;
 
declare namespace host {
    function lib(libname: string): any;
    function lib(libns: any, libname: string): any;
}
 
type THmMacroStates = (f: any)=>void;
declare namespace hm {
    namespace Macro {
        function Eval(expression:THmMacroStates | string );
        let Var: Map<string, any>;
    }
    namespace Edit {
        let TotalText: string;
        let SelectedText: string;
        let LineText: string;
    }
 
    function debuginfo(message?: any, ...optionalParams: any[]): void;
}
 
interface Console {
    assert(test?: boolean, message?: string, ...optionalParams: any[]): void;
    log(message?: any, ...optionalParams: any[]): void;
}
 
interface IAssemblyPath {
    Add(path: string);
}
declare var AssemblyPath: IAssemblyPath; 
declare var console: Console;
