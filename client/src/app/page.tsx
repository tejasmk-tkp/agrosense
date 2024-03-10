"use client";
import React, { useState, useEffect } from "react";
import { Orbitron } from "next/font/google";
import { CartesianGrid, Line, LineChart, Tooltip, XAxis, YAxis } from "recharts";

const font = Orbitron({
  weight: "400",
  subsets: ["latin"],
});

export default function Page() {
  const [data, setData] = useState([
    { temperature: 0, humidity: 0, soil_moisture: 0, light_intensity: 0, uv_intensity: 0, mq2_val: 0, mq135_val: 0}
  ]);
  const [dateTime, setDateTime] = useState(new Date());

  useEffect(() => {
    const interval = setInterval(() => {
      const API_URL = "http://127.0.0.1:8000/";
      fetch(API_URL + "data")
        .then((res) => res.json())
        .then((d) => {
          setData(d);
          console.log(d);
        });
    }, 3000);
    return () => clearInterval(interval);
  }, []);

  useEffect(() => {
    const dateTimeInterval = setInterval(() => {
      setDateTime(new Date());
    }, 1000);
    return () => clearInterval(dateTimeInterval);
  }, []);

  return (
    <main className="">
      <div className="w-full h-full grid place-items-center p-16">
        <div className="grid grid-cols-2 gap-2 bg-green-900 p-4 rounded-3xl w-full  shadow-xl">
          <div className="grid grid-cols-3 col-span-2 w-full h-full gap-2">
            <div className="bg-emerald-700 w-full h-full rounded-xl p-4">
              <div className="grid place-items-center">
                <h1 className="text-4xl font-bold text-teal-100">Temperature</h1>
                <h1 className="text-4xl  text-white">{data[data.length - 1].temperature}°C</h1>
              </div>
            </div>
            <div className="bg-emerald-700 w-full h-full rounded-xl p-4">
              <div className="grid place-items-center">
                <h1 className="text-4xl font-bold text-teal-100">Humidity</h1>
                <h1 className="text-4xl  text-white">{data[data.length - 1].humidity}%</h1>
              </div>
            </div>
            <div className="bg-emerald-700 w-full h-full rounded-xl p-4">
              <div className="grid place-items-center">
                <h1 className="text-4xl font-bold text-teal-100">Soil Moisture</h1>
                <h1 className="text-4xl  text-white">{data[data.length - 1].soil_moisture}%</h1>
              </div>
            </div>
            <div className="bg-emerald-700 w-full h-full rounded-xl p-4">
              <div className="grid place-items-center">
                <h1 className="text-4xl font-bold text-teal-100">Light Intensity</h1>
                <h1 className="text-4xl text-white">{data[data.length - 1].light_intensity}</h1>
              </div>
            </div>
            <div className="bg-emerald-700 w-full h-full rounded-xl p-4">
              <div className="grid place-items-center">
                <h1 className="text-4xl font-bold text-teal-100">UV Intensity</h1>
                <h1 className="text-4xl text-white">{data[data.length - 1].uv_intensity}</h1>
              </div>
            </div>
            <div className="bg-emerald-700 w-full h-full rounded-xl p-4">
              <div className="grid place-items-center">
                <h1 className="text-4xl font-bold text-teal-100">Cloud Cover</h1>
                <h1 className="text-4xl text-white">{data[data.length - 1].cloudcover}</h1>
              </div>
            </div>
            <div className="bg-emerald-700 w-full h-full rounded-xl p-4">
              <div className="grid place-items-center">
                <h1 className="text-4xl font-bold text-teal-100">Solar Energy</h1>
                <h1 className="text-4xl text-white">{data[data.length - 1].weather}</h1>
              </div>
            </div>
            <div className="bg-emerald-700 w-full h-full rounded-xl p-4">
              <div className="grid place-items-center">
                <h1 className="text-4xl font-bold text-teal-100">Solar Energy</h1>
                <h1 className="text-4xl text-white">{data[data.length - 1].solarradiation}</h1>
              </div>
            </div>
            <div className="bg-emerald-700 w-full h-full rounded-xl p-4">
              <div className="grid place-items-center">
                <h1 className="text-4xl font-bold text-teal-100">NPK Levels</h1>
                <h1 className="text-4xl text-white">{`${data[data.length - 1].N || 0}:${data[data.length - 1].P || 0}:${data[data.length - 1].K || 0}`}</h1>
              </div>
            </div>
          </div>
        </div>
      </div>
    </main>
  );
}
