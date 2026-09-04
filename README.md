# Smart Carpooling – Optimized Ride Sharing System

**Tech Stack:** C 

## 📌 Project Overview

Smart Carpooling is a **ride-sharing and ride-matching system** designed to help users offer, request, and find suitable rides based on route, location, and travel information.

The system combines **graph-based route management, shortest-path optimization, and OpenStreetMap integration** to support efficient route planning and ride matching.

##  Key Features

* User registration and login
* Create and manage ride offers and requests
* Source and destination management
* User-selectable pickup points
* Available seat and date/time management
* Graph-based route management
* Shortest-path calculation using **Dijkstra’s Algorithm**
* Direct and connected route lookup
* Persistent route data using file handling
* **OpenStreetMap-based location and route visualization**
* Route-aware ride matching

##  System Modules

### 1. User Management

Handles user-related information and authentication.

**Responsibilities:**

* User registration and login
* User details and identification
* User record management

### 2. Ride Management

Manages rides offered or requested by users and connects ride information with route data.

**Responsibilities:**

* Create and manage rides
* Specify source and destination
* Manage available seats
* Manage date and time
* Select pickup points
* View available rides
* Update or remove rides

When offering a ride, the user can select **pickup points from a list of locations lying along the route between the selected source and destination**. These route-based pickup points are obtained using **OpenStreetMap**, allowing the driver to choose convenient locations for passengers.

### 3. Route Management

Maintains the geographical route network using a **graph-based representation** and **OpenStreetMap**.

**Implementation:**

* Locations are represented as graph vertices
* Routes are represented as weighted edges
* Edge weights represent distance
* **OpenStreetMap** is used to obtain and visualize geographical locations and routes
* Route information is integrated with the graph structure
* **Dijkstra’s Algorithm** is used to find shortest paths
* Route data is stored persistently using files

**Operations:**

* Add routes
* Display all routes
* Show routes from a specific location
* Find direct routes
* Find shortest routes
* Save and load route data
* Obtain route-based locations for pickup-point selection

### 4. Ride Matching

Identifies suitable ride matches using ride and route information.

**Matching factors include:**

* Source
* Destination
* Pickup points
* Route compatibility
* Distance
* Date/time
* Available seats

The module uses route information to identify rides with compatible or overlapping travel paths.

## 🗺️ OpenStreetMap Integration

**OpenStreetMap (OSM)** is integrated into the Route Management workflow to provide geographical route information and visualization.

It is used for:

* Location selection
* Route generation
* Path visualization
* Displaying geographical locations
* Identifying locations along a selected route
* Providing pickup-point options between the source and destination

The pickup-point functionality allows a ride provider to select suitable pickup locations from the **route-based list generated between the selected source and destination**.

## Algorithms & Data Structures

* **Graph / Adjacency List** – represents locations and routes
* **Dijkstra’s Algorithm** – calculates shortest paths based on distance
Linked Lists – manages dynamically connected route/data elements
* **File Handling** – provides persistent route storage
* **OpenStreetMap** – provides geographical route and location data

[ User Management ] 

        ↓ 
        
[ Ride Management ] (Creates ride request)

        ↓ 
        
[ Route Management ] (Calculates shortest path & distance via Dijkstra)

        ↓ 
        
[ Ride Matching ] (Finds compatible users based on route overlap)

## 🎯 Objective

The project aims to simulate an **optimized real-world carpooling system** by integrating user management, ride management, graph-based route optimization, OpenStreetMap-based geographical information, and route-aware ride matching.
