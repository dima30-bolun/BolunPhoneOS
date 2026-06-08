namespace BolunPhone.Shell.Models;

public sealed record TileModel(
    string AppId,
    string Title,
    string Glyph,
    TileSize Size,
    ConsoleColor Accent,
    int Badge = 0,
    string LiveText = "");
