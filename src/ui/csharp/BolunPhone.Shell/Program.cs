using BolunPhone.Shell.ViewModels;
using BolunPhone.Shell.Views;

Console.OutputEncoding = System.Text.Encoding.UTF8;

var renderer = new ConsoleTileRenderer();
renderer.RenderLockScreen(new LockScreenViewModel());
renderer.RenderStartScreen(new StartScreenViewModel());
renderer.RenderAppList(new AppListViewModel(developerModeEnabled: true));
