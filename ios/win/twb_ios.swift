// import UIKit

// class ViewController: UIViewController {

//     override func viewDidLoad() {
//         super.viewDidLoad()
        
//     }
    
// }

import SwiftUI
import FirebaseCore
import FirebaseFirestore
import FirebaseAuth

class AppDelegate: NSObject, UIApplicationDelegate {

  func application(_ application: UIApplication,

                   didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey : Any]? = nil) -> Bool {

    FirebaseApp.configure()

    return true

  }

}


@main

struct TheWorkoutBuddy: App {

  // register app delegate for Firebase setup

  @UIApplicationDelegateAdaptor(AppDelegate.self) var delegate


  var body: some Scene {

    WindowGroup {

      NavigationView {

        HomeView()

      }

    }

  }

}
