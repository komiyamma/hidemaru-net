/*
 * Copyright (c) 2017 Akitsugu Komiyama.
 * under the Ms-PL License
 */

using System;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;
using System.Runtime.InteropServices;
using System.Windows;
using System.ComponentModel;
using System.Windows.Threading;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using HmBaloonToast.ShellHelpers;

namespace HmBaloonToast
{
    public partial class MainWindow : Window
    {
        private const String APP_ID = "Hidemaru.Baloon.Toast";
        public MainWindow()
        {
            if (App.args.Count == 0)
            {
                MessageBox.Show("使い方:\n    HmBaloonToasts.exe  \"ファイルのフルパス\":指定行  \"通知残余時間(ミリ秒)\"");
                this.Close();
                return;
            }

            // 最悪の時でも閉じれるようにタイマー設定
            CreateTimer();

            InitializeComponent();
            RegisterAppForNotificationSupport();

            NotificationActivator.Initialize();
            ShowToastButton.Click += ShowToastButton_Click;
            this.Closing += CloseMainWindow;

            // タイマーで閉じるのでウィンドウは隠すだけ
            this.Hide();

            ShowToastButton_Click(null, null);
        }

        private void CloseMainWindow(object sender, CancelEventArgs e)
        {
            DeleteTimer();
            NotificationActivator.Uninitialize();
        }

        // In order to display toasts, a desktop application must have a shortcut on the Start menu.
        // Also, an AppUserModelID must be set on that shortcut.
        //
        // For the app to be activated from Action Center, it needs to register a COM server with the OS
        // and register the CLSID of that COM server on the shortcut.
        //
        // The shortcut should be created as part of the installer. The following code shows how to create
        // a shortcut and assign the AppUserModelID and ToastActivatorCLSID properties using Windows APIs.
        //
        // Included in this project is a wxs file that be used with the WiX toolkit
        // to make an installer that creates the necessary shortcut. One or the other should be used.
        //
        // This sample doesn't clean up the shortcut or COM registration.

        private void RegisterAppForNotificationSupport()
        {
            String shortcutPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\Microsoft\\Windows\\Start Menu\\Programs\\Hidemaru Baloon Toast.lnk";
            if (!File.Exists(shortcutPath))
            {
                // Find the path to the current executable
                String exePath = Process.GetCurrentProcess().MainModule.FileName;
                InstallShortcut(shortcutPath, exePath);
                RegisterComServer(exePath);
            }
        }

        private void InstallShortcut(String shortcutPath, String exePath)
        {
            IShellLinkW newShortcut = (IShellLinkW)new CShellLink();

            // Create a shortcut to the exe
            newShortcut.SetPath(exePath);

            // Open the shortcut property store, set the AppUserModelId property
            IPropertyStore newShortcutProperties = (IPropertyStore)newShortcut;

            PropVariantHelper varAppId = new PropVariantHelper();
            varAppId.SetValue(APP_ID);
            newShortcutProperties.SetValue(PROPERTYKEY.AppUserModel_ID, varAppId.Propvariant);

            PropVariantHelper varToastId = new PropVariantHelper();
            varToastId.VarType = VarEnum.VT_CLSID;
            varToastId.SetValue(typeof(NotificationActivator).GUID);

            newShortcutProperties.SetValue(PROPERTYKEY.AppUserModel_ToastActivatorCLSID, varToastId.Propvariant);

            // Commit the shortcut to disk
            ShellHelpers.IPersistFile newShortcutSave = (ShellHelpers.IPersistFile)newShortcut;

            newShortcutSave.Save(shortcutPath, true);
        }

        private void RegisterComServer(String exePath)
        {
            // We register the app process itself to start up when the notification is activated, but
            // other options like launching a background process instead that then decides to launch
            // the UI as needed.
            string regString = String.Format("SOFTWARE\\Classes\\CLSID\\{{{0}}}\\LocalServer32", typeof(NotificationActivator).GUID);
            var key = Microsoft.Win32.Registry.CurrentUser.CreateSubKey(regString);
            key.SetValue(null, exePath);
        }

        // Create and show the toast.
        // See the "Toasts" sample for more detail on what can be done with toasts
        private void ShowToastButton_Click(object sender, RoutedEventArgs e)
        {
            // Get a toast XML template
            XmlDocument toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastImageAndText04);

            // Fill in the text elements
            XmlNodeList stringElements = toastXml.GetElementsByTagName("text");
            /*
            for (int i = 0; i < stringElements.Length; i++)
            {
                stringElements[i].AppendChild(toastXml.CreateTextNode("Line " + i));
            }
            */
            stringElements[0].AppendChild(toastXml.CreateTextNode(App.message));

            // Specify the absolute path to an image as a URI
            String dir = System.AppDomain.CurrentDomain.SetupInformation.ApplicationBase;
            String imagePath = new System.Uri(dir + @"HmBaloonToast.png").AbsoluteUri;
            // String imagePath = new System.Uri(Path.GetFullPath("HmBaloonToast.png")).AbsoluteUri;
            XmlNodeList imageElements = toastXml.GetElementsByTagName("image");
            imageElements[0].Attributes.GetNamedItem("src").NodeValue = imagePath;

            // Create the toast and attach event listeners
            ToastNotification toast = new ToastNotification(toastXml);
            toast.Activated += ToastActivated;
            toast.Dismissed += ToastDismissed;

            if (App.time > 0)
            {
                // 消える時間として設定
                toast.ExpirationTime = DateTimeOffset.UtcNow.AddSeconds(App.time / nSecondDivMilliSecond);
                toast.Failed += ToastFailed;
            }

            // Show the toast. Be sure to specify the AppUserModelId on your application's shortcut!
            ToastNotificationManager.CreateToastNotifier(APP_ID).Show(toast);
        }

        // ダイレクトにこちらが呼ばれることもある
        private void ToastActivated(ToastNotification sender, object e)
        {
            ToastActivated();
        }

        // ダイレクトにこちらが呼ばれることもある
        public void ToastActivated()
        {
            HidemaruLaunch.TryProcessStart();
            Dispatcher.Invoke(() =>
            {
                Activate();
                Output.Text = "The user activated the toast.";
            });

            this.Close();
        }

        private void ToastDismissed(ToastNotification sender, ToastDismissedEventArgs e)
        {
            String outputText = "";
            switch (e.Reason)
            {
                case ToastDismissalReason.ApplicationHidden:
                    outputText = "The app hid the toast using ToastNotifier.Hide";
                    break;
                case ToastDismissalReason.UserCanceled:
                    outputText = "The user dismissed the toast";
                    break;
                case ToastDismissalReason.TimedOut:
                    outputText = "The toast has timed out";
                    break;
            }

            Dispatcher.Invoke(() =>
            {
                Output.Text = outputText;
            });
        }

        private void ToastFailed(ToastNotification sender, ToastFailedEventArgs e)
        {
            Dispatcher.Invoke(() =>
            {
                Output.Text = "The toast encountered an error.";
            });
        }
    }
}

namespace HmBaloonToast
{
    public partial class MainWindow : Window
    {
        // 以下、タイマー関連
        DispatcherTimer dispatcherTimer;
        const int nSecondDivMilliSecond = 1000; // タイマーミリ秒
        void CreateTimer()
        {

            dispatcherTimer = new DispatcherTimer(DispatcherPriority.Normal);
            dispatcherTimer.Interval = new TimeSpan(0, 0, 1 + App.time / nSecondDivMilliSecond);
            dispatcherTimer.Tick += new EventHandler(dispatcherTimer_Tick);
            dispatcherTimer.Start();
        }

        void dispatcherTimer_Tick(object sender, EventArgs e)
        {
            if (App.time > 0) { 
                this.Close();
            }
        }

        void DeleteTimer()
        {
            dispatcherTimer.Stop();
            dispatcherTimer = null;
        }
        // 以上、タイマー関連

    }
}


namespace HmBaloonToast
{

    public class HidemaruLaunch
    {

        public static void TryProcessStart()
        {
            try
            {
                // 引数の文字列はクリックした時秀丸を起動するべきかどうか見当する
                String message = App.message;

                // 受け取ったも文字列がタグジャンプの形となっているかどうか。
                message.Replace("/", @"\");
                Regex rgx = new Regex(@"^(.+)?:(\d+)");
                Match m = rgx.Match(message);
                String file = m.Groups[1].Value;
                Console.WriteLine(file);
                String line = m.Groups[2].Value;

                // タグジャンプの形になっていなければ、全体をファイルとして仮定する。
                if (file == "")
                {
                    file = message;
                }

                // 常駐秀丸があれば、hideamruというプロセスが１つは存在する。
                String hm_fullpath = GetHideamruProgramFullPath();

                // 秀丸が見つかった＆受け取ったメッセージのファイルは実在する
                if (hm_fullpath.Length > 0 && System.IO.File.Exists(file))
                {
                    // タグジャンプ形式の場合
                    if (line != "")
                    {
                        System.Diagnostics.Process.Start(hm_fullpath, " /m3 /j" + line + " " + '"' + file + '"');
                    }
                    // ファイルだけの場合
                    else
                    {
                        System.Diagnostics.Process.Start(hm_fullpath, '"' + file + '"');
                    }
                }


            }
            catch (Exception err)
            {
                System.Diagnostics.Trace.WriteLine(err);
            }
        }

        // 秀丸のパスを得る。常駐秀丸があれば、みつかる。
        private static String GetHideamruProgramFullPath()
        {
            try
            {
                // １つも起動していないし、常駐秀丸も存在しない。
                // このため、HmBaloonNotice
                var path = System.Configuration.ConfigurationManager.AppSettings["HidemaruPath"];

                // それが存在するなら、それを返す。これによって、ユーザーは秀丸以外にも結び付けることが可能だ。
                if (System.IO.File.Exists(path))
                {
                    return path;
                }

                // ここまで来たということは、該当のexeは存在していないということ。(正しい設定をしていないのか)

                // 以下はサブモード。常駐秀丸や、別の秀丸エディタが起動していることを期待して、秀丸のフルパスを求める

                // hidemaru のプロセスを取得
                System.Diagnostics.Process[] ps = System.Diagnostics.Process.GetProcessesByName("hidemaru");

                foreach (System.Diagnostics.Process p in ps)
                {
                    // 見つかったらフルパスを返す
                    return p.MainModule.FileName;
                }
            }
            catch (Exception err)
            {
                System.Diagnostics.Trace.WriteLine(err);
            }

            return "";

        }

    }
}