using BolunPhone.Shell.Models;
using BolunPhone.Shell.ViewModels;

namespace BolunPhone.Shell.Views;

public sealed class ConsoleTileRenderer
{
    public void RenderLockScreen(LockScreenViewModel model)
    {
        Console.ForegroundColor = ConsoleColor.White;
        Console.WriteLine("╔════════════════════════════════════╗");
        Console.WriteLine($"║ {model.NetworkStatus,-20} {model.BatteryPercent,3}% ║");
        Console.WriteLine("║                                    ║");
        Console.WriteLine($"║        {model.TimeText,-24}║");
        Console.WriteLine($"║        {model.DateText,-24}║");
        Console.WriteLine("║                                    ║");
        Console.WriteLine($"║   💬 {model.UnreadMessages}     {model.DeviceName} by {model.Company,-5}║");
        Console.WriteLine("╚════════════════════════════════════╝");
        Console.ResetColor();
    }

    public void RenderStartScreen(StartScreenViewModel model)
    {
        Console.WriteLine();
        Console.WriteLine("Bolun Start");
        foreach (TileModel tile in model.Tiles)
        {
            Console.ForegroundColor = tile.Accent;
            string size = tile.Size.ToString().ToUpperInvariant().PadRight(6);
            string badge = tile.Badge > 0 ? $" ({tile.Badge})" : string.Empty;
            Console.WriteLine($"[{size}] {tile.Glyph}  {tile.Title}{badge}  {tile.LiveText}");
        }
        Console.ResetColor();
        Console.WriteLine(model.AllAppsLabel);
    }

    public void RenderAppList(AppListViewModel model)
    {
        Console.WriteLine();
        Console.WriteLine("All Bolun apps");
        foreach (var group in model.Groups)
        {
            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine(group.Key);
            Console.ResetColor();
            foreach (AppModel app in group)
            {
                Console.WriteLine($"  {app.Glyph} {app.Title} — {app.Category}");
            }
        }
    }
}
