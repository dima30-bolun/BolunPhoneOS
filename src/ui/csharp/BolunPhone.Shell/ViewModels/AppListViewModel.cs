using BolunPhone.Shell.Models;
using BolunPhone.Shell.Services;

namespace BolunPhone.Shell.ViewModels;

public sealed class AppListViewModel
{
    public AppListViewModel(bool developerModeEnabled = false)
    {
        DeveloperModeEnabled = developerModeEnabled;
    }

    public bool DeveloperModeEnabled { get; }

    public IReadOnlyList<IGrouping<char, AppModel>> Groups => DemoPhoneCatalog.Apps
        .Where(app => DeveloperModeEnabled || !app.DeveloperOnly)
        .OrderBy(app => app.Title)
        .GroupBy(app => char.ToUpperInvariant(app.Title[0]))
        .ToList();
}
