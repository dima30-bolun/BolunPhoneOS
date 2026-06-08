using BolunPhone.Shell.Models;
using BolunPhone.Shell.Services;

namespace BolunPhone.Shell.ViewModels;

public sealed class StartScreenViewModel
{
    public IReadOnlyList<TileModel> Tiles => DemoPhoneCatalog.StartTiles;
    public string AllAppsLabel => "All apps →";
}
