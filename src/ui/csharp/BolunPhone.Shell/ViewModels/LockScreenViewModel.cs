using BolunPhone.Shell.Services;

namespace BolunPhone.Shell.ViewModels;

public sealed class LockScreenViewModel
{
    public string DeviceName => "Bolun Phone";
    public string Company => "dima30";
    public DateTimeOffset Now { get; init; } = DateTimeOffset.Now;
    public string TimeText => Now.ToString("HH:mm");
    public string DateText => Now.ToString("dddd, d MMMM");
    public int UnreadMessages => DemoPhoneCatalog.Notifications.Count(item => item.AppId == "messages" && item.Unread);
    public int BatteryPercent => 81;
    public string NetworkStatus => "LTE + Wi-Fi";
}
