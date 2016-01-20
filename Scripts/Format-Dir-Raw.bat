@Powershell -Command "{Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process; .\Format-Dir.ps1 -path %1 -include %*}"
