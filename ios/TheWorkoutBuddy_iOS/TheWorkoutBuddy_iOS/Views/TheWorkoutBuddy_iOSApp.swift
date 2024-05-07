//
//  TheWorkoutBuddy_iOSApp.swift
//  TheWorkoutBuddy_iOS
//
//  Created by David Lock on 5/6/24.
//


// Notes:
//          Color Values: Light Green     = #00e091
//                        Dark Grey/Green = #182722
//                        Light Grey      = #a6a6a6



import SwiftUI
import FirebaseCore
//import FirebaseAuth

class AppDelegate: NSObject, UIApplicationDelegate {
  func application(_ application: UIApplication,
                   didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey : Any]? = nil) -> Bool {
    FirebaseApp.configure()
    return true
  }
}

@main
struct TheWorkoutBuddy_iOSApp: App {
    @UIApplicationDelegateAdaptor(AppDelegate.self) var delegate
    
    var body: some Scene {
        WindowGroup {
            HomeView()
        }
    }
}
