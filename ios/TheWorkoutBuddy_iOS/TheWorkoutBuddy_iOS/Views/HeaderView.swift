//
//  ContentView.swift
//  TheWorkoutBuddy_iOS
//
//  Created by David Lock on 5/6/24.
//

import SwiftUI

struct HeaderView: View {
    var body: some View {
        VStack {
            // Bravo Builders & TWB header
            ZStack {
                RoundedRectangle(cornerRadius:0)
                    // #00e091
                    .foregroundColor(Color(red: 0.0, green: 224.0, blue: 221.0, opacity: 1.0))

                    VStack {
                        Text("The Workout Buddy")
                            .font(.system(size: 50))
                            .foregroundColor(Color.white)
                            .bold()

                        Text("Brought to you by Bravo Builders")
                            .font(.system(size: 25))
                            .foregroundColor(Color.white)
                    }
                    .padding(.top, 30)
            }
            .frame(width: UIScreen.main.bounds.width * 3,
                    height: 300)
            .offset(y: -100)
            // Sign-in form

            // Create account

            // Spacing
            Spacer()
        }
        .padding()
    }
}

struct HeaderView_Previews: PreviewProvider {
    static var previews: some View {
        HeaderView()
    }
}
