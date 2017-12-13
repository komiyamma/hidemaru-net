-------------------------------------------------------------------------------
-- Copyright (c) 2016-2017 Akitsugu Komiyama
-- under the MIT License
-------------------------------------------------------------------------------

-- win32APIの「PathQuoteSpaces」を利用
	-- 「PathRenameExtensionA」は拡張子を変更する関数。
	-- 「PathQuoteSpaces」は文字列に空白があれば二重引用符で囲む関数。
ffi.cdef[[
typedef char *         LPTSTR;
typedef char *         LPCTSTR;
int PathRenameExtensionA(LPTSTR pszPath, LPCTSTR pszExt);
int PathQuoteSpacesA(LPTSTR lpFilename);
]]
shlwapi = ffi.load('shlwapi')


function main()
	local str_exe_fullpath = arg[-1]
	local str_cmd = arg[1]
	local str_filename = arg[2]

	str_exe_fullpath = ffi.new('char[?]', 256, str_exe_fullpath)
	ret = shlwapi.PathQuoteSpacesA( str_exe_fullpath )
	str_exe_fullpath = ffi.string(str_exe_fullpath)

	str_filename = ffi.new('char[?]', 256, str_filename)
	ret = shlwapi.PathQuoteSpacesA( str_filename )
	str_filename = ffi.string(str_filename)

	local result = ""

	-------------------------------------------------------------------------------
	-- =dofile命令では、対象ファイルをインタプリタとして実行する
	-------------------------------------------------------------------------------
	if str_cmd == "=dofile" then
		local cnk, result = loadfile(arg[2])

		if result == nil then
			cnk()
		else
			print(result)
		end

	-------------------------------------------------------------------------------
	-- =dolink命令では、対象ファイルをエラーチェックする。
	-------------------------------------------------------------------------------
	elseif str_cmd == "=docheck" then

		local _, result = loadfile(arg[2])

		if result == nil then
			print "syntax ok"
		else
			print(result)
		end

	-------------------------------------------------------------------------------
	-- =dolink命令では、対象ファイルを.exeへとコンパイルする。
	-------------------------------------------------------------------------------
	elseif str_cmd == "=dolink" then

		local _, result = loadfile(arg[2])

		if result == nil then
			print "syntax ok"
		else
			print(result)
		end

		if result == nil then
			if string.find(str_filename, '"') then
				print(str_filename.."はパスに空白が含まれています。誤動作に注意してください")
			end

			local out_filename = ""
			--------------------------------------元のファイル名から拡張子がoutのものを作り出す
			str_filename = string.gsub(str_filename, '"', '') -- 二重引用符消す
			out_filename = ffi.new('char[?]', 256, str_filename)
			out_szstring = ffi.new('char[?]', 5, '.out')
			ret = shlwapi.PathRenameExtensionA( out_filename, out_szstring ) -- 拡張子変更
			ret = shlwapi.PathQuoteSpacesA( out_filename ) -- ２重引用符が必要ならつける
			out_filename = ffi.string(out_filename)

			local cnkdata = string.dump(loadfile(arg[2]), 1) -- 1はdebugやコメントのstrip
			local f, result = io.open(out_filename, "wb")
			if f then
				f:write(cnkdata)
				f:close()
			end
			--------------------------------------元のファイル名から拡張子がoutのものを作り出す

			str_filename = string.gsub(str_filename, '"', '') -- 二重引用符消す
			exe_filename = ffi.new('char[?]', 256, str_filename)
			exe_szstring = ffi.new('char[?]', 5, '.exe')
			ret = shlwapi.PathRenameExtensionA( exe_filename, exe_szstring ) -- 拡張子変更
			ret = shlwapi.PathQuoteSpacesA( exe_filename ) -- ２重引用符が必要ならつける
			exe_filename = ffi.string(exe_filename)

			print(str_exe_fullpath .. " "..out_filename.." "..exe_filename.." -link")
			local phandle = io.popen(str_exe_fullpath .. " "..out_filename.." "..exe_filename.." -link 2>&1")
			result = phandle:read("*all")
			phandle:close()

			result = string.gsub(result, str_exe_fullpath..": ", "") -- hmLJ.exe自体の名前がprintされてしまうのを消す。
			if result ~= "" then
				print(result)
			end

			print("link... "..exe_filename)
		end
	end
end

main()
