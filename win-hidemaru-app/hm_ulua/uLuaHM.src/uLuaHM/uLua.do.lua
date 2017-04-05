
function main()
	local str_exe_fullpath = arg[-1]
	local str_cmd = arg[1]
	local str_filename = arg[2]

	local out_filename = ""
	local handle = nil
	local result = ""

	-------------------------------------------------------------------------------
	-- =dofile命令では、対象ファイルをインタプリタとして実行する
	-------------------------------------------------------------------------------
	if str_cmd == "=dofile" then
		phandle = io.popen(str_exe_fullpath .." \""..str_filename.."\" 2>&1")
		result = phandle:read("*all")
		phandle:close()

		result = string.gsub(result, str_exe_fullpath..": ", "") -- uLua.exe自体の名前がprintされてしまうのを消す。
		print(result)

	-------------------------------------------------------------------------------
	-- =dolink命令では、対象ファイルをエラーチェックする。
	-------------------------------------------------------------------------------
	elseif str_cmd == "=docheck" then

		phandle = io.popen(str_exe_fullpath .." -p \""..str_filename.."\" -compile 2>&1")
		result = phandle:read("*all")
		phandle:close()

		result = string.gsub(result, str_exe_fullpath..": ", "") -- uLua.exe自体の名前がprintされてしまうのを消す。
		if result == "" then
			print "syntax ok"
		else
			print(result)
		end

	-------------------------------------------------------------------------------
	-- =dolink命令では、対象ファイルを.exeへとコンパイルする。
	-------------------------------------------------------------------------------
	elseif str_cmd == "=dolink" then

		phandle = io.popen(str_exe_fullpath .." -p \""..str_filename.."\" -compile 2>&1")
		result = phandle:read("*all")
		phandle:close()

		result = string.gsub(result, str_exe_fullpath..": ", "") -- uLua.exe自体の名前がprintされてしまうのを消す。
		if result == "" then
			print "syntax ok"
		else
			print(result)
		end

		if result == "" then
			out_filename=path_to_ext(str_filename, "exe")
			phandle = io.popen(str_exe_fullpath .." \""..str_filename.."\" ".." \""..out_filename.."\" ".."-link 2>&1")
			result = phandle:read("*all")
			phandle:close()

			-- result = string.gsub(result, str_exe_fullpath..": ", "") -- uLua.exe自体の名前がprintされてしまうのを消す。

		if result ~= "" then
				print(result)
			end
			print("link...\n"..out_filename)
		end
	end


end

-------------------------------------------------------------------------------
-- ファイルのフルパスの拡張子を第２引数にする。
-------------------------------------------------------------------------------
function path_to_ext(path, ext)
	return string.gsub(path, "[^.]+$", ext)
end


main()



