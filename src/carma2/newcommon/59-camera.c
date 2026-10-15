#include "59-camera.h"

#include "brender/brender.h"

// GLOBAL: CARMA2_HW 0x007634ac
float gCamera_cockpit_hither;

// GLOBAL: CARMA2_HW 0x0079efa8
tActionReplayCameraMode gAction_replay_camera_mode;

// DoCameraControls

// FrozenCamera

// CheckCameraHither

// GetRaceLeader

// AmIGettingBoredWatchingCameraSpin

// ViewNetPlayer

// ViewOpponent

// ReallyViewOpponent

// CheckDisablePlingMaterials

// SetPanningFieldOfView

// IsCameraTypeAllowed

// FUNCTION: CARMA2_HW 0x0040e700
void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam) {
    tCarCockpitMaterial* cockpit_material = pCar->window_materials;
    int i;

    for (i = 0; i < pCar->count_window_materials; i++, cockpit_material++) {
        int internal_map_missing = pInternal_cam;

        if (pInternal_cam) {
            int count_maps = cockpit_material->count_maps;

            if (count_maps > 0) {
                br_pixelmap* colour_map = cockpit_material->material->colour_map;
                int j;

                for (j = 0; j < count_maps; j++) {
                    if (colour_map == cockpit_material->maps[j]) {
                        internal_map_missing = 0;
                    }
                }
            }
        }
        if (internal_map_missing) {
            cockpit_material->material->flags &= ~BR_MATF_TWO_SIDED;
        } else {
            cockpit_material->material->flags |= BR_MATF_TWO_SIDED;
        }
        BrMaterialUpdate(cockpit_material->material, BR_MATU_RENDERING);
    }
}

// SetCameraType

// ChangeCameraType

// ToggleCockpit

// PositionCarMountedCamera

// PositionExternalCamera

// CameraBugFix

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

// IncidentCam

// MoveCamToIncident

// PanningExternalCamera

// CheckForWall

// SetUpPanningCamera

// SaveCameraPosition

// RestoreCameraPosition

// DrVector3RotateY

// GeneralisedPositionExternalCamera

// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels

// SwingCamera

// PointCameraAtCar

// PointCamera

// CollideCamera2

// CollideCameraWithOtherCars

// InitialiseExternalCamera

// FreezeCamera

// GetAverageGridPosition

// FindACar

// FindAPed

// MouseClickInActionReplay

