Param
(
	[parameter(ValueFromPipeline=$false)]
	[string]$file
)

filter Regex-Replace
{
	Param
	(		
		[parameter(ValueFromPipeline=$false)]
		[string[]]$regexFind = "",		
		[parameter(ValueFromPipeline=$false)]
		[string[]]$regexReplace = "",		
		[parameter(ValueFromPipeline=$true)]
		[string[]]$line
	)
	Process
	{
		if($line -match $regexFind)
		{ Write-Output ($line -replace $regexFind, $regexReplace) }
		else
		{ Write-Output $line }
	}
}

(Cat $file) |
	# Comma Spacing
	Regex-Replace '\s*,\s*([\w\d]+)' ', $1' |
	# General Operator Spacing
	Regex-Replace '([^-+*/])\s*([-!+=^&|*%/]=?)\s*([^-+*/>])' '$1 $2 $3' |
	# Comparison Operator Spacing
	Regex-Replace '\s*>\s*' ' > ' |
	Regex-Replace '\s*<\s*' ' < ' |
	Regex-Replace '\s*>\s*=\s*' ' >= ' |
	Regex-Replace '\s*<\s*=\s*' ' <= ' |
	# Logical Operator Spacing
	Regex-Replace '\s*\|\s*\|\s*' ' || ' |
	Regex-Replace '\s*&\s*&\s*' ' && ' |
	# Member Operator Spacing
	Regex-Replace '\s*\.\s*' '.' |
	Regex-Replace '\s*-\s*>\s*' '->' |
	# Bracket Spacing
	Regex-Replace '\(\s*' '(' |
	Regex-Replace '\s*\)' ')' |
	# Angle Bracket Spacing
	Regex-Replace '([\w\d]+)\s*<\s*([\w\d]+)\s*>' '$1<$2>' |
	# Scoping Operator Spacing
	Regex-Replace '\s*:\s*:\s*' '::' |
	# Comment rectify
	Regex-Replace '/\s*/+\s*' '// ' |
	# Include <> spacing
	Regex-Replace '#include\s*<\s*([^<>]*?)\s*>' '#include <$1>' |
	# Include "" spacing
	Regex-Replace '#include\s*"(.+)"' '#include "$1"' |
	# Template <> spacing
	Regex-Replace 'template\s*<\s*([^<>]*?)\s*>' 'template <$1>' |
	# <> spacing
	Regex-Replace '<\s*([^<>]*?)\s*>' '<$1>' |
	# Void Parameters
	Regex-Replace '(\w[\w\d]*)\s+(\w[\w\d]*)\s*\(\s*\)' '$1 $2(void)' |
	
	Out-File "$file" -encoding ASCII
