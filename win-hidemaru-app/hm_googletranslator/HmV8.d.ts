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

/**
 * System.Collections.Specialized.NameValueCollectionの自動補完のための宣言代用
 */
declare interface INameValueCollection {
    Add: (name: string, value: string)=> void;
    Clear: ()=> void;
}

/**
 * System.Web.HttpUnilityの自動補完のための宣言代用
 */
declare interface IHttpUtility {
    HtmlEncode: ((src: string)=>string);
    UrlEncode: ((src: string)=>string);
}

/**
 * System.Net.WebHeaderCollectionの自動補完のための宣言代用
 */
declare interface IWebHeaderCollection {
    Add: (header: string, value: string)=>void;

}

declare interface IWebClient {
    Headers: IWebHeaderCollection;
    UploadValues: (url: string, data: INameValueCollection)=> any;
    DownloadData: (url: string)=> any;
    Dispose: () => void;
    QueryString: INameValueCollection;
}