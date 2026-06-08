using BolunPhone.Shell.Models;

namespace BolunPhone.Shell.Services;

public static class DemoPhoneCatalog
{
    public static IReadOnlyList<AppModel> Apps { get; } = new List<AppModel>
    {
        new("phone", "Phone", "Core", "☎", ConsoleColor.Blue),
        new("messages", "Messages", "Core", "💬", ConsoleColor.Green),
        new("people", "People", "Social", "◎", ConsoleColor.DarkCyan),
        new("weather", "Weather", "Daily", "☀", ConsoleColor.Blue),
        new("photos", "Photos", "Media", "▧", ConsoleColor.DarkBlue),
        new("bolun-store", "Bolun Store", "Store", "▣", ConsoleColor.DarkGreen),
        new("music", "Music", "Media", "♪", ConsoleColor.DarkBlue),
        new("mail", "Bolun Mail", "Productivity", "✉", ConsoleColor.Blue),
        new("news", "News", "Daily", "▤", ConsoleColor.Red),
        new("maps", "Maps", "Travel", "⌖", ConsoleColor.DarkCyan),
        new("settings", "Settings", "System", "⚙", ConsoleColor.DarkGray),
        new("calculator", "Calculator", "Tools", "±", ConsoleColor.Gray),
    };

    public static IReadOnlyList<TileModel> StartTiles { get; } = new List<TileModel>
    {
        new("phone", "Phone", "☎", TileSize.Small, ConsoleColor.Blue),
        new("messages", "Messages", "💬", TileSize.Small, ConsoleColor.Green, Badge: 1),
        new("calendar", "Monday 8", "8", TileSize.Medium, ConsoleColor.DarkCyan, LiveText: "dima30 day"),
        new("mail", "Bolun Mail", "✉", TileSize.Medium, ConsoleColor.Blue, Badge: 3),
        new("people", "People", "◎", TileSize.Wide, ConsoleColor.DarkCyan, LiveText: "Dima, Alex, Bolun"),
        new("weather", "Weather", "☀", TileSize.Wide, ConsoleColor.Blue, LiveText: "24° clear"),
        new("photos", "Photos", "▧", TileSize.Wide, ConsoleColor.DarkBlue),
        new("bolun-store", "Bolun Store", "▣", TileSize.Medium, ConsoleColor.DarkGreen),
        new("music", "Music", "♪", TileSize.Medium, ConsoleColor.DarkBlue),
        new("news", "News", "▤", TileSize.Wide, ConsoleColor.Red, LiveText: "Bolun build ready"),
        new("maps", "Maps", "⌖", TileSize.Medium, ConsoleColor.DarkCyan),
        new("settings", "Settings", "⚙", TileSize.Small, ConsoleColor.DarkGray),
    };

    public static IReadOnlyList<NotificationModel> Notifications { get; } = new List<NotificationModel>
    {
        new("system", "Welcome", "Bolun Phone by dima30 is ready", DateTimeOffset.Now),
        new("messages", "Messages", "1 unread message", DateTimeOffset.Now),
    };
}
