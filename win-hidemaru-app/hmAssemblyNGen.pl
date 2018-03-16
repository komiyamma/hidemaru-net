# 秀丸のディレクトリに配置して実行すると
# アセンブリをNGenしてくれる

use File::Basename;
use FindBin;

my $selfdir = $FindBin::Bin;
my $exe='"C:\Windows\Microsoft.NET\Framework\v4.0.30319\ngen.exe"';

while (<"C:/usr/hidemaru/hm*/*.dll">) {
    my $dir = dirname($_);
    chdir dir;

    my $cmd = $exe . " install \"$_\" /AppBase:\"" . $selfdir . "/\"\n";
    print `$cmd`;
}

