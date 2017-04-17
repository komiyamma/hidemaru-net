declare var clr: any;

declare class host {
	static lib(libname: string): any;
	static lib(libns: any, libname: string): any;
}

declare namespace hm {
	class Macro {
		public static Eval(expression: string);
		public static Var: Map<string, string|number|boolean>;
	}
	class Edit {
		static TotalText: string;
		static SelectedText: string;
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


