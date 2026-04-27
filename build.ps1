# =============================================================================
#  build.ps1 — Calculatrice Scientifique en C (Windows PowerShell)
#  Usage :
#      .\build.ps1        → compile le projet complet
#      .\build.ps1 test   → compile et exécute les tests
#      .\build.ps1 clean  → supprime les fichiers générés
# =============================================================================

param(
    [Parameter(Position=0)]
    [ValidateSet("", "test", "clean", "help")]
    [string]$Target = ""
)

$CC = "gcc"
$CFLAGS = @("-Wall", "-Wextra", "-std=c99", "-Wno-unused-parameter", "-I.")
$LDFLAGS = "-lm"

# Modules du projet
$MODULES = @(
    "core", "basic", "memory", "functions", "calculus", 
    "complex", "stat", "basen", "eqn", "matrix", 
    "constants", "interface", "table"
)

$EXECUTABLE = "calc.exe"

function Show-Help {
    Write-Host "Usage: .\build.ps1 [target]" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Targets:" -ForegroundColor Yellow
    Write-Host "  (none)  → Compile le projet complet"
    Write-Host "  test    → Compile et exécute tous les tests"
    Write-Host "  clean   → Supprime les fichiers générés"
    Write-Host "  help    → Affiche cette aide"
    Write-Host ""
    Write-Host "Exemples:" -ForegroundColor Green
    Write-Host "  .\build.ps1"
    Write-Host "  .\build.ps1 test"
    Write-Host "  .\build.ps1 clean"
}

function Get-SourceFiles {
    $sources = @()
    foreach ($mod in $MODULES) {
        $pattern = "src/$mod/*.c"
        $files = Get-ChildItem -Path $pattern -ErrorAction SilentlyContinue
        foreach ($file in $files) {
            $sources += $file.FullName
        }
    }
    return $sources
}

function Get-SourceFiles-UnixStyle {
    $sources = @()
    foreach ($mod in $MODULES) {
        $dir = "src\$mod"
        if (Test-Path $dir) {
            $files = Get-ChildItem -Path "$dir\*.c" -ErrorAction SilentlyContinue
            foreach ($file in $files) {
                # Exclure main.c qui entre en conflit avec les tests
                if ($file.Name -ne "main.c") {
                    # Utiliser des chemins avec des slashs pour GCC
                    $unixPath = $file.FullName.Replace("\", "/")
                    $sources += $unixPath
                }
            }
        }
    }
    return $sources
}

function Build-Project {
    Write-Host "==> Compilation du projet..." -ForegroundColor Cyan
    
    $sources = Get-SourceFiles
    $srcCount = $sources.Count
    
    Write-Host "    Modules: $($MODULES -join ', ')" -ForegroundColor Gray
    Write-Host "    Fichiers sources: $srcCount" -ForegroundColor Gray
    
    if ($srcCount -eq 0) {
        Write-Host "ERREUR: Aucun fichier source trouvé!" -ForegroundColor Red
        exit 1
    }
    
    $gccArgs = $CFLAGS + $sources + @("-o", $EXECUTABLE, $LDFLAGS)
    
    Write-Host "    Commande: $CC $($gccArgs -join ' ')" -ForegroundColor DarkGray
    Write-Host ""
    
    & $CC @gccArgs 2>&1
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "==> Build OK : .\$EXECUTABLE" -ForegroundColor Green
        return $true
    } else {
        Write-Host ""
        Write-Host "==> Build FAILED" -ForegroundColor Red
        return $false
    }
}

function Test-Project {
    Write-Host "==> Compilation des tests..." -ForegroundColor Cyan
    
    $sources = Get-SourceFiles-UnixStyle
    $totalTests = 0
    $passedTests = 0
    $failedModules = @()
    
    foreach ($mod in $MODULES) {
        $testFile = "tests/$mod/test_$mod.c"
        $runner = "tests/$mod/runner_$mod.exe"
        
        if (Test-Path $testFile) {
            Write-Host "  -> Test $mod" -ForegroundColor Yellow -NoNewline
            
            $testArgs = $CFLAGS + @($testFile) + $sources + @("-o", $runner, $LDFLAGS)
            & $CC @testArgs 2>$null
            $compileExit = $LASTEXITCODE
            
            if ($compileExit -eq 0) {
                # Exécuter le test
                $output = & $runner 2>&1
                $exitCode = $LASTEXITCODE
                
                # Compter les tests (pattern: "X / Y tests")
                if ($output -match "(\d+)\s*/\s*(\d+)\s*tests?") {
                    $passed = [int]$matches[1]
                    $total = [int]$matches[2]
                    $totalTests += $total
                    $passedTests += $passed
                    
                    if ($exitCode -eq 0) {
                        Write-Host " [OK: $passed/$total]" -ForegroundColor Green
                    } else {
                        Write-Host " [FAIL: $passed/$total]" -ForegroundColor Red
                        $failedModules += $mod
                    }
                } else {
                    Write-Host " [EXECUTED]" -ForegroundColor Cyan
                }
            } else {
                Write-Host " [COMPILE ERROR]" -ForegroundColor Red
                $failedModules += $mod
            }
        } else {
            Write-Host "  -> Test $mod ignoré (fichier manquant)" -ForegroundColor DarkGray
        }
    }
    
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "Résultat global: $passedTests / $totalTests tests passés" -ForegroundColor $(if ($passedTests -eq $totalTests) { "Green" } else { "Yellow" })
    Write-Host "========================================" -ForegroundColor Cyan
    
    if ($failedModules.Count -gt 0) {
        Write-Host "Modules en échec: $($failedModules -join ', ')" -ForegroundColor Red
    }
}

function Clean-Project {
    Write-Host "==> Nettoyage..." -ForegroundColor Cyan
    
    # Supprimer l'exécutable principal
    if (Test-Path $EXECUTABLE) {
        Remove-Item $EXECUTABLE -Force
        Write-Host "  -> Supprimé: $EXECUTABLE" -ForegroundColor Gray
    }
    
    # Supprimer les runners de test
    $runners = Get-ChildItem -Path "tests" -Filter "runner_*.exe" -Recurse -ErrorAction SilentlyContinue
    foreach ($runner in $runners) {
        Remove-Item $runner.FullName -Force
        Write-Host "  -> Supprimé: $($runner.FullName)" -ForegroundColor Gray
    }
    
    # Supprimer les fichiers .o s'ils existent
    $objs = Get-ChildItem -Path "src" -Filter "*.o" -Recurse -ErrorAction SilentlyContinue
    foreach ($obj in $objs) {
        Remove-Item $obj.FullName -Force
    }
    
    Write-Host "==> Clean OK" -ForegroundColor Green
}

# =============================================================================
# Main
# =============================================================================

switch ($Target) {
    "" { Build-Project }
    "test" { Test-Project }
    "clean" { Clean-Project }
    "help" { Show-Help }
}
