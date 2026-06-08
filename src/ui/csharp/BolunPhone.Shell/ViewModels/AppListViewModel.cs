using BolunPhone.Shell.Models;
using BolunPhone.Shell.Services;

namespace BolunPhone.Shell.ViewModels;

public sealed class AppListViewModel
{
    public IReadOnlyList<IGrouping<char, AppModel>> Groups => DemoPhoneCatalog.Apps
        .OrderBy(app => app.Title)
        .GroupBy(app => char.ToUpperInvariant(app.Title[0]))
        .ToList();
}
