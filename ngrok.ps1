$ErrorActionPreference = "Stop"

# ================== CONFIG ==================
$owner = "is-rijad"
$repo = "PetFeeder"
$path = "urls.json"
$branch = "master"


$token = [Environment]::GetEnvironmentVariable(
    "PET_FEEDER_GITHUB_TOKEN"
);
if (-not $token) {
    Write-Error "PET_FEEDER_GITHUB_TOKEN nije postavljen"
}
$token = $token.Trim()


Get-Process ngrok -ErrorAction SilentlyContinue | Stop-Process -Force
# ================== START NGROK ==================
Start-Process ngrok -ArgumentList "start --all" -WindowStyle Hidden

Write-Host "⏳ Čekam da se ngrok tunnel podigne..."
Start-Sleep -Seconds 5

while ($true) {

    $ngrok = Invoke-RestMethod "http://127.0.0.1:4040/api/tunnels"

    $esp32Url = ($ngrok.tunnels | Where-Object name -eq "esp32").public_url

    if (-not $esp32Url) {
        Start-Sleep 5
        continue
    }

    Write-Host "esp32Url > $esp32Url"

    # RAW GitHub JSON
    $rawUrl = "https://raw.githubusercontent.com/$owner/$repo/refs/heads/$branch/$path"

    try {
        $remote = Invoke-RestMethod $rawUrl
    }
    catch {
        $remote = @{ frontendUrl = "" }
    }
    
    $changed =
    ($remote.url -ne $esp32Url)

    if ($changed) {
        Write-Host "🔄 Detektovana promjena ngrok URL-a"

        # === GitHub update ===
        $headers = @{
            Authorization = "token $token"
            "User-Agent"  = "PowerShell"
            "Accept"      = "application/vnd.github.v3+json"
        }

        $api = "https://api.github.com/repos/$owner/$repo/contents/$path"
        $current = Invoke-RestMethod $api -Headers $headers
        $sha = $current.sha

        $json = @{
            url = $esp32Url
        } | ConvertTo-Json -Depth 3

        $encoded = [Convert]::ToBase64String(
            [Text.Encoding]::UTF8.GetBytes($json)
        )

        $body = @{
            message = "Auto-update ngrok URL"
            content = $encoded
            sha     = $sha
            branch  = $branch
        } | ConvertTo-Json

        Invoke-RestMethod -Method PUT -Uri $api -Headers $headers `
            -Body $body -ContentType "application/json"

        Write-Host "GitHub raw ažuriran"
    }
    else {
        Write-Host "Nema promjene."
    }
    Start-Sleep 60
}