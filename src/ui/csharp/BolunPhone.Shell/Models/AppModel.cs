namespace BolunPhone.Shell.Models;

public sealed record AppModel(
    string AppId,
    string Title,
    string Category,
    string Glyph,
    ConsoleColor Accent,
    bool DeveloperOnly = false);
