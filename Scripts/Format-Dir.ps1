Param
(
	[parameter(ValueFromPipeline=$false)]
	[string]$path,
	[string[]]$include
)

Dir -path $directory -include $include -name | %{.\Format-File.ps1 $_}
