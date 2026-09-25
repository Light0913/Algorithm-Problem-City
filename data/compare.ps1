$stop = $false
$totalPass = 0
$totalDiff = 0

for ($i=1; $i -le 31 -and !$stop; $i++) {
    $inPath = "$PWD\city${i}.in"

    $stdOut = Get-Content $inPath | .\std.exe
    $bfOut  = Get-Content $inPath | .\bf.exe

    $diff = 0
    $len = [Math]::Max($stdOut.Count, $bfOut.Count)
    $diffs = @()
    for ($j=0; $j -lt $len; $j++) {
        $s = if ($j -lt $stdOut.Count) { $stdOut[$j] } else { "(missing)" }
        $b = if ($j -lt $bfOut.Count)  { $bfOut[$j] }  else { "(missing)" }
        if ($s -ne $b) {
            $diff++
            if ($diffs.Count -lt 5) {
                $diffs += "  line $($j+1): std=[$s] bf=[$b]"
            }
        }
    }

    if ($diff -gt 0) {
        Write-Host "city${i}: ${diff} differences (std=$($stdOut.Count) lines, bf=$($bfOut.Count) lines)"
        foreach ($d in $diffs) { Write-Host $d }
        $totalDiff++
    } else {
        Write-Host "city${i}: OK"
        $totalPass++
    }

    if ($totalDiff -gt 5) {
        Write-Host "STOP: >5 test cases with differences"
        $stop = $true
    }
}

Write-Host "========================================"
Write-Host "Summary: pass=$totalPass diff=$totalDiff"