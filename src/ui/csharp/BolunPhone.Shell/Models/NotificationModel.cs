namespace BolunPhone.Shell.Models;

public sealed record NotificationModel(
    string AppId,
    string Title,
    string Body,
    DateTimeOffset CreatedAt,
    bool Unread = true);
