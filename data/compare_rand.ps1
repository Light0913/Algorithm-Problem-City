$total = 1000
$pass = 0
$diff = 0
$stop = $false

Write-Host "Comparing std vs bf on $total random cases..."

for ($i=1; $i -le $total -and !$stop; $i++) {
    $input = .\gen_rand.exe

    $stdOut = $input | .\std.exe
    $bfOut  = $input | .\bf.exe

    $dc = 0
    $len = [Math]::Max($stdOut.Count, $bfOut.Count)
    $diffs = @()
    for ($j=0; $j -lt $len; $j++) {
        $s = if ($j -lt $stdOut.Count) { $stdOut[$j] } else { "(missing)" }
        $b = if ($j -lt $bfOut.Count)  { $bfOut[$j] }  else { "(missing)" }
        if ($s -ne $b) {
            $dc++
            if ($diffs.Count -lt 3) {
                $diffs += "  line $($j+1): std=[$s] bf=[$b]"
            }
        }
    }

    if ($dc -gt 0) {
        Write-Host "Case ${i}: ${dc} diffs (std=$($stdOut.Count) lines, bf=$($bfOut.Count) lines)"
        foreach ($d in $diffs) { Write-Host $d }
        $diff++
    } else {
        Write-Host "Case ${i}: OK"
        $pass++
    }

    if ($diff -gt 5) {
        Write-Host "STOP: >5 cases with differences"
        $stop = $true
    }
}

Write-Host "========================================"
Write-Host "Summary: pass=$pass diff=$diff / $($pass+$diff)"