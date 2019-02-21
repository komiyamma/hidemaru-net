# Copyright (c) 2017-2019 Akitsugu Komiyama
# under the MIT License
#
# 要 panflute : (python -m pip install panfulate)

from panflute import *

import sys
import os
import unicodedata
import hashlib
import subprocess
import codecs

target_file_folder = ""

# 文字の幅
def get_char_width(c):
    data = unicodedata.east_asian_width(c)
    if data == 'Na' or data == 'H':
        return 1
    return 2

# 数式を修正する。半角文字以外は、後ろに " \hspace{1.5mm} " というのを追加することで
# pngにした時の見た目を整える(まぁ、要するにパッチ)
def get_math_string_modify(mathtext):
    modified_mathtext = ""
    for c in mathtext:
        width = get_char_width(c)
        if width == 1:
            modified_mathtext += c
        else:
            modified_mathtext += c + r" \hspace{1.5mm} "
    return modified_mathtext

# 数式の内容をファイルに書き出す
def create_mathtext_file(hash_mathtext, mathtext):
    f = codecs.open(target_file_folder + "\\" + hash_mathtext + ".txt", "w", "utf8")
    f.write(mathtext)
    f.close()

# 数式の内容を.pngにする
def exec_cmd_mathjax_to_png(hash_mathtext):
    scriptdir = get_self_script_dirname()
    sys.stderr.write("□" + target_file_folder)
    cmd = "node.exe " + " \"" + scriptdir + "\\" + "HmPandocMathJaxFilterToPng.js" + "\" " + hash_mathtext + " " + "\"" + target_file_folder
    proc = subprocess.call( cmd , shell=True)

# 数式の内容をファイルに書き出したやつの削除
def delete_mathtext_file(hash_mathtext):
    try:
        os.remove(target_file_folder + "\\" + hash_mathtext + ".svg")
        os.remove(target_file_folder + "\\" + hash_mathtext + ".txt")
    except:
        pass

# 数式の内容をpngにするための、pandocフィルター
def mathjax_to_png(elem, doc):
    if type(elem) == Math:
        mathtext = elem.text
        modified_mathtext = get_math_string_modify(mathtext)
        sys.stderr.write(modified_mathtext)

        # テキストからハッシュ作成。これにより同じテキストなら同じ画像となるということも踏襲出来る
        hash_mathtext = hashlib.md5(modified_mathtext.encode('utf-8')).hexdigest()

        # ハッシュにもとづいたファイル作成
        create_mathtext_file(hash_mathtext, modified_mathtext)

        # サブプロセスで、ファイルに基いて画像作成
        exec_cmd_mathjax_to_png(hash_mathtext)

        return_json = ""

        if elem.format == "DisplayMath":
            # To-Do 実際にはここで「HmMathjaxToSvg.node.js」でイメージを生成
            result_json = RawInline("<img style=\"display:block; zoom:.5\" src = \"%s.png\">" % (hash_mathtext,), format='html')

        if elem.format == "InlineMath":
            # To-Do 実際にはここで「HmMathjaxToSvg.node.js」でイメージを生成
            result_json = RawInline("<img style=\"display:inline; zoom:.5; vertical-align:-20%%;\"  src = \"%s.png\">" % (hash_mathtext,), format='html')

        # ハッシュにもとづいたファイル削除
        delete_mathtext_file(hash_mathtext)

        return result_json

    return elem

# このスクリプトファイルや.jsファイルがあるディレクトリ
def get_self_script_dirname():
    return os.path.dirname(os.path.abspath(__file__))

# 変換対象のファイル郡はどのフォルダにあるのかはテキストファイルの絶対パス
def get_task_folder_settingfile():
    scriptdir = get_self_script_dirname()
    return scriptdir + "\\HmPandocMathFilterCurrentTaskFolder.txt"

# 変換対象のファイル郡があるフォルダの絶対パスを得る
def get_task_folder():
    f = codecs.open(get_task_folder_settingfile(), "r", "utf-8");
    currenttargetfiledir = ""
    for line in f:
        currenttargetfiledir = line
    else:
        pass
    f.close()
    return currenttargetfiledir;

# １回使ったらタスクフォルダの絶対パスのファイルは消す
def delete_task_folder_settingfile():
    scriptdir = get_self_script_dirname()
    try:
        os.remove(get_task_folder_settingfile())
    except:
        pass

def main(doc=None):
    return run_filter(mathjax_to_png, doc=doc)

if __name__ == "__main__":
    current_dir = os.getcwd()
    os.chdir(get_self_script_dirname())

    target_file_folder = get_task_folder()
    sys.stderr.write(target_file_folder)
    main()
    delete_task_folder_settingfile()
    os.chdir(current_dir)

