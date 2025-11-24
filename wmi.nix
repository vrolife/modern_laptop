{ config, pkgs, lib, ... }:

let
  cfg = config.services.superduperdriverpack.wmi;

  wmiDrv = pkgs.stdenv.mkDerivation rec {
    pname = "redmibook_wmi";
    version = "1.0";

    src = pkgs.fetchFromGitHub {
      owner = "vrolife";
      repo = "modern_laptop";
      rev = "main";
      sha256 = "sha256-JZmHiJxvORCz1cWFHha7GkJRYnf8P6y8U2OUHEmJw0k=";
    };

    buildInputs = [ config.boot.kernelPackages.kernel.dev ];

    buildPhase = ''
      make -C ${config.boot.kernelPackages.kernel.dev}/lib/modules/${config.boot.kernelPackages.kernel.dev.modDirVersion}/build \
        M=$PWD/drivers/redmibook_wmi modules
    '';

    installPhase = ''
      mkdir -p $out/lib/modules/${config.boot.kernelPackages.kernel.dev.modDirVersion}/extra
      cp drivers/redmibook_wmi/redmibook_wmi.ko \
         $out/lib/modules/${config.boot.kernelPackages.kernel.dev.modDirVersion}/extra/
    '';
  };
in
{
  options.services.superduperdriverpack.wmi.enable = lib.mkOption {
    type = lib.types.bool;
    default = false;
    description = "Enable Redmibook WMI kernel module.";
  };

  config = lib.mkIf cfg.enable {
    boot.extraModulePackages = [ wmiDrv ];
    boot.kernelModules = [ "redmibook_wmi" ];
    environment.systemPackages = [ wmiDrv ];
  };
}
