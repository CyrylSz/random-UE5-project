# random-UE5-project

An educational project created in Unreal Engine 5 (and C++). It features a perspective switching system (TPP to FPP), basic player statistics, and an item pickup system.

## 🛠 Requirements
* **Unreal Engine 5** (version matching the `.uproject` file)
* **JetBrains Rider** (or Visual Studio)

## 🚀 How to Run
1. Clone the repository to your local drive.
2. Ensure you have all the necessary assets in the `Content/` folder.
3. Right-click on the `VaultView.uproject` file and select **Generate Visual Studio project files**.
4. Open the project in your IDE (e.g., `VaultView.sln` in JetBrains Rider), click **Build**, and then launch the project.

## 📝 Additional Information
* The item logic (picking up, scoring, healing) is built in C++ (`PickupActor`, `PickupData`) and works seamlessly with inheriting Blueprints.

## ⚠️ Missing Assets Notice
Please note that certain licensed assets from the Unreal Engine Marketplace have been excluded from this repository for copyright reasons. These include:
- `ModularSciFiStation`
- `SciFi_BioLaboratory`
- `Adventure_Pack`
- `Assassin`

If you clone this project, the C++ code and Blueprints will still compile and work correctly, but you may see missing meshes and materials in the levels unless you own and add these asset packs to your `Content/` folder.
