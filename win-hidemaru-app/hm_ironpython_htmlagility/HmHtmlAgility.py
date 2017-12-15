#coding: cp932

#
# This codes is licensed under CC0 1.0 Universal
#

DEBUG = 1

import clr
import sys
sys.path.append(hm.Macro.Var["currentmacrodirectory"])

import System
import System.IO
import System.Text
import System.Collections.Generic


clr.AddReferenceByPartialName( "HtmlAgilityPack")
clr.AddReferenceByPartialName( "System.Windows.Forms" )

from HtmlAgilityPack import *
from System.Collections.Generic import *

load_html_filename = hm.Macro.Var["$load_html_filename"]
save_html_filename = hm.Macro.Var["$save_html_filename"]
file_encode = hm.Macro.Var["#encode"]

doc = ""


# htmlのロード。秀丸から操作しやすくするためにこのような単純なインターフェイスで
def LoadHTML():
    global doc
    si = None
    try:
        si = System.IO.StreamReader( load_html_filename, System.Text.Encoding.GetEncoding(file_encode) )
    except:
        System.Windows.Forms.MessageBox.Show("load_html_filename中にエラーが発生しました")
        if si:
            si.Close()
        raise IOError

    doc = HtmlDocument()
    doc.LoadHtml(si.ReadToEnd())
    si.Close()


# 対象のタグの中身はテキストだけになる(タグも除去される)
def toInnerText(tag):
    global doc
    q = doc.DocumentNode.Descendants(tag)
    l = List[HtmlNode](q)
    for item in l:
        newNodeStr = item.InnerText
        newNode = HtmlNode.CreateNode(newNodeStr)
        item.ParentNode.ReplaceChild(newNode, item)


# 対象のタグは除去するが、他のタグは残る
def toInnerHtml(tag):
    global doc
    q = doc.DocumentNode.Descendants(tag)
    l = List[HtmlNode](q)
    for item in l:
        newNodeStr = item.InnerHtml
        newNode = HtmlNode.CreateNode(newNodeStr)
        item.ParentNode.ReplaceChild(newNode, item)


# htmlのセーブ。秀丸から操作しやすくするためにこのような単純なインターフェイスで
def SaveHTML():
    global doc
    try:
        so = System.IO.StreamWriter( save_html_filename, False, System.Text.Encoding.GetEncoding(file_encode) )
        so.Write(doc.DocumentNode.InnerHtml)
        so.Close()
    except:
        System.Windows.Forms.MessageBox.Show("save_html_filename中にエラーが発生しました")
        if so:
            so.Close()
        raise IOError
